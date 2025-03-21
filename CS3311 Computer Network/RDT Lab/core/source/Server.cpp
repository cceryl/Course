#include "Server.h"

#include <filesystem>
#include <fstream>

using std::exception, std::ifstream, std::ios;
using std::move, std::format, std::min, std::max, std::clamp, std::abs;
using std::shared_ptr, std::static_pointer_cast;
using std::thread, std::lock_guard, std::mutex, std::unique_lock;
using std::chrono::milliseconds, std::chrono::high_resolution_clock, std::uint64_t, std::vector;
using std::filesystem::path, std::filesystem::exists, std::filesystem::file_size;

Server::Server(Socket::Domain domain, int port, milliseconds timeout, std::size_t retries) :
    mTimeout(timeout), mRetries(retries), mChannel(domain, port), mRunning(false)
{
    mChannel.setRetryCallback([this](Channel::Task task) { onRetry(move(task)); });
    mChannel.setTimeoutCallback([this](Channel::Task task) { onTimeout(move(task)); });
    mChannel.setReceiveCallback([this](Channel::Message message) { onReceive(move(message)); });
}

Server::~Server() { this->stop(); }

void Server::setTaskCallback(TransferCallback callback) noexcept { mTaskCallback = callback; }

void Server::setFinishCallback(TransferCallback callback) noexcept { mFinishCallback = callback; }

void Server::setErrorCallback(ErrorCallback callback) noexcept { mErrorCallback = callback; }

void Server::start()
{
    mRunning = true;
    mThread  = thread([this]() { worker(); });
}

void Server::stop()
{
    mRunning = false;
    if (mThread.joinable())
    {
        mCondition.notify_one();
        mThread.join();
    }

    mTasks.clear();
}

void Server::worker() noexcept
{
    static auto isEnd = [](ifstream &file, auto size) { return file.eof() || static_cast<decltype(size)>(file.tellg()) == size; };
    static auto work  = [this](const Client &client, Task &task) -> bool
    {
        if (task.packets.size() >= task.window)
            return false;

        while (task.packets.size() < task.window && !isEnd(task.file, task.size))
        {
            uint64_t begin = task.file.tellg(), end = min(begin + sBlockSize, task.size);
            Data     data(end - begin);
            task.file.read(reinterpret_cast<char *>(data.data()), end - begin);
            task.checksum.update(data);

            auto         protocol = DataProtocol(task.path, begin / sBlockSize, move(data));
            auto         handle   = mChannel.send({protocol.serialize(), client.IP, client.port}, task.timeout, mRetries);
            Task::Packet packet   = {handle, high_resolution_clock::now()};
            task.packets.insert({begin / sBlockSize, packet});
        }

        if (isEnd(task.file, task.size))
        {
            task.checksum.finalize();
            auto checksum = task.checksum.digest();
            mMD5Cache.insert({task.path, checksum});

            if (task.packets.empty())
                return true;
        }

        return false;
    };

    while (mRunning)
    {
        unique_lock<mutex> lock(mMutex);
        mCondition.wait(lock, [this]() { return !mTasks.empty() || !mRunning; });

        if (!mRunning)
            return;

        vector<Client> complete;

        for (auto &[client, task] : mTasks)
            try
            {
                if (work(client, task))
                {
                    if (mFinishCallback)
                        mFinishCallback(client, task.path);
                    complete.push_back(client);
                }
            }
            catch (const exception &e)
            {
                if (mErrorCallback)
                    mErrorCallback(format("Error when processing task for {}:{} - {}", client.IP, client.port, e.what()));
                complete.push_back(client);
            }

        for (auto &client : complete)
            this->terminate(client);
    }
}

void Server::terminate(Client client)
{
    if (!mTasks.contains(client))
        return;

    auto &task = mTasks.at(client);
    for (auto &[block, packet] : task.packets)
        mChannel.terminate(packet.handle);

    mTasks.erase(client);
}

void Server::onRetry(Channel::Task task)
{
    lock_guard lock(mMutex);
    if (mTasks.contains({task.IP, task.port}))
    {
        auto &window = mTasks.at({task.IP, task.port}).window;
        window       = max(window / 2, static_cast<size_t>(1));
    }
}

void Server::onTimeout(Channel::Task task)
{
    if (!mRunning)
        return;

    lock_guard lock(mMutex);
    this->terminate({task.IP, task.port});
    if (mErrorCallback)
        mErrorCallback(format("Timeout on {}:{} with maximum retries, connection terminated", task.IP, task.port));
}

void Server::onReceive(Channel::Message message)
{
    if (!mRunning)
        return;

    try
    {
        auto protocol = Protocol::deserialize(message.data);
        auto client   = Client{message.IP, message.port};

        switch (protocol->type())
        {
        case Protocol::Type::Query:       onQuery(client, static_pointer_cast<QueryProtocol>(protocol)); break;
        case Protocol::Type::Get:         onGet(client, static_pointer_cast<GetProtocol>(protocol)); break;
        case Protocol::Type::Acknowledge: onAcknowledge(client, static_pointer_cast<AcknowledgeProtocol>(protocol)); break;
        case Protocol::Type::Checksum:    onChecksum(client, static_pointer_cast<ChecksumProtocol>(protocol)); break;
        default:                          break;
        }
    }
    catch (const exception &e)
    {
        if (mErrorCallback)
            mErrorCallback(format("Error when receiving message from {}:{}", message.IP, message.port));
    }
}

void Server::onQuery(Client client, shared_ptr<QueryProtocol> protocol)
{
    path file = protocol->path();
    if (!exists(file))
    {
        auto error = ErrorProtocol(protocol->path(), "File does not exist");
        mChannel.send({error.serialize(), client.IP, client.port}, mTimeout, mRetries);
        return;
    }

    auto size     = static_cast<uint64_t>(file_size(file));
    auto response = SizeProtocol(protocol->path(), size);
    mChannel.send({response.serialize(), client.IP, client.port});
}

void Server::onGet(Client client, shared_ptr<GetProtocol> protocol)
{
    path file = protocol->path();
    if (!exists(file))
    {
        auto error = ErrorProtocol(protocol->path(), "File does not exist");
        mChannel.send({error.serialize(), client.IP, client.port});
        return;
    }

    {
        lock_guard lock(mMutex);

        auto start = StartProtocol(protocol->path());
        mChannel.send({start.serialize(), client.IP, client.port});

        if (mTasks.find(client) != mTasks.end())
            return;

        mTasks.insert({client, Task(protocol->path(), static_cast<uint64_t>(file_size(file)), ifstream(file, ios::binary), 1, mTimeout)});

        if (mTaskCallback)
            mTaskCallback(client, protocol->path());
    }

    mCondition.notify_one();
}

void Server::onAcknowledge(Client client, shared_ptr<AcknowledgeProtocol> protocol)
{
    {
        lock_guard lock(mMutex);
        if (!mTasks.contains(client))
            return;

        auto &task = mTasks.at(client);
        auto  iter = task.packets.find(protocol->block());
        if (iter == task.packets.end())
            return;

        auto [block, packet] = *iter;
        mChannel.terminate(packet.handle);
        task.packets.erase(block);

        if (task.window < sSlowStart)
            task.window = min(task.window * 2, sSlowStart);
        else if (task.window < sMaxWindow)
            task.window += 1;

        milliseconds elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - packet.departure);
        task.average         = milliseconds((task.average.count() * 7 + elapsed.count()) / 8);
        task.variance        = milliseconds((task.variance.count() * 3 + abs(task.average.count() - elapsed.count())) / 4);
        task.timeout         = clamp(task.average + task.variance * 4, milliseconds(100), mTimeout);
    }

    mCondition.notify_one();
}

void Server::onChecksum(Client client, shared_ptr<ChecksumProtocol> protocol)
{
    lock_guard lock(mMutex);

    auto iter = mTasks.find(client);
    if (iter != mTasks.end())
        this->terminate(client);

    if (!mMD5Cache.contains(protocol->path()))
    {
        path file = protocol->path();
        if (!exists(file))
        {
            auto error = ErrorProtocol(protocol->path(), "File does not exist");
            mChannel.send({error.serialize(), client.IP, client.port});
            return;
        }

        MD5      checksum;
        ifstream stream(file, ios::binary);
        checksum.compute(stream);

        mMD5Cache.insert({protocol->path(), checksum.digest()});
    }

    bool correct  = mMD5Cache.at(protocol->path()) == protocol->checksum();
    Data response = correct ? ConfirmProtocol(protocol->path()).serialize() : ErrorProtocol(protocol->path(), "Checksum mismatch").serialize();
    mChannel.send({response, client.IP, client.port});
}
