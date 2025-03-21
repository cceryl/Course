#include "Client.h"

using std::function, std::make_shared, std::static_pointer_cast;
using std::lock_guard, std::unique_lock, std::condition_variable;
using std::size_t, std::move, std::ios, std::exception, std::vector;
using std::string, std::string_view, std::format, std::chrono::milliseconds;
using std::chrono::high_resolution_clock, std::chrono::duration_cast;

Client::Client(Socket::Domain domain, int port, milliseconds timeout, size_t retries) :
    mTimeout(timeout), mRetries(retries), mState(State::Idle), mChannel(domain, port), mDuplicate(0)
{
    mChannel.setTimeoutCallback([this](Channel::Task task) { onTimeout(move(task)); });
    mChannel.setReceiveCallback([this](Channel::Message message) { onReceive(move(message)); });
}

void Client::setProgressCallback(ProgressCallback callback) noexcept { mProgressCallback = callback; }

void Client::setFinishCallback(FinishCallback callback) noexcept { mFinishCallback = callback; }

void Client::setErrorCallback(ErrorCallback callback) noexcept { mErrorCallback = callback; }

void Client::download(string_view IP, int port, string_view file, string_view destination)
{
    while (!mProtocols.empty())
        mProtocols.pop();
    if (mOutput.is_open())
        mOutput.close();
    mCache.clear();
    mChecksum.reset();

    mIP          = IP;
    mPort        = port;
    mFile        = file;
    mDestination = destination;
    mStart       = high_resolution_clock::now();

    mState = State::Query;
    this->dispatch();
}

Client::Stats Client::statistics() const noexcept
{
    return Stats{mChannel.corrupted(), mDuplicate, mChannel.lost() - mDuplicate, mChannel.successful()};
}

void Client::dispatch()
{
    try
    {
        while (true)
            switch (mState)
            {
            case State::Idle:        return;
            case State::Query:       this->query(); break;
            case State::Size:        this->size(); break;
            case State::Get:         this->get(); break;
            case State::Start:       this->start(); break;
            case State::Data:        this->data(); break;
            case State::Acknowledge: this->acknowledge(); break;
            case State::Checksum:    this->checksum(); break;
            case State::Confirm:     this->confirm(); break;
            case State::Error:       this->error(); break;
            default:                 return;
            }
    }
    catch (const exception &e)
    {
        mErrorCallback(e.what());
        mState = State::Idle;
    }
}

bool Client::wait(Protocol::Type type)
{
    unique_lock lock(mMutex);
    mCondition.wait(lock,
                    [this, type]
                    {
                        if (mState == State::Idle)
                            return true;

                        while (!mProtocols.empty())
                        {
                            auto protocol = mProtocols.front();
                            if (protocol->type() == type || protocol->type() == Protocol::Type::Error)
                                return true;

                            mProtocols.pop();
                        }

                        return false;
                    });

    if (mState == State::Idle)
        return false;

    mChannel.terminate(mHandle);

    if (mProtocols.front()->type() == Protocol::Type::Error)
    {
        mState = State::Error;
        return false;
    }

    return true;
}

void Client::onTimeout(Channel::Task task)
{
    mErrorCallback(format("Failed to send message to {}:{} with maximum retries", task.IP, task.port));
    mChannel.terminate(mHandle);
    mState = State::Idle;

    mCondition.notify_one();
}

void Client::onReceive(Channel::Message message)
{
    {
        lock_guard lock(mMutex);

        try
        {
            mProtocols.push(Protocol::deserialize(message.data));
        }
        catch (const exception &e)
        {
            mErrorCallback(e.what());
            mState = State::Idle;
        }
    }

    mCondition.notify_one();
}

void Client::query()
{
    auto protocol = QueryProtocol(mFile);
    mHandle       = mChannel.send({protocol.serialize(), mIP, mPort}, mTimeout, mRetries);
    mState        = State::Size;
}

void Client::size()
{
    if (!this->wait(Protocol::Type::Size))
        return;

    lock_guard lock(mMutex);

    auto protocol = static_pointer_cast<SizeProtocol>(mProtocols.front());
    mProtocols.pop();
    mSize  = protocol->size();
    mState = State::Get;
}

void Client::get()
{
    auto protocol = GetProtocol(mFile);
    mHandle       = mChannel.send({protocol.serialize(), mIP, mPort}, mTimeout, mRetries);
    mState        = State::Start;
}

void Client::start()
{
    if (!this->wait(Protocol::Type::Start))
        return;

    lock_guard lock(mMutex);

    auto protocol = static_pointer_cast<StartProtocol>(mProtocols.front());
    mProtocols.pop();
    mOutput.open(mDestination, ios::binary);
    mBlock = 0;
    mNext  = 0;
    mChecksum.reset();
    mState = State::Data;

    if (!mOutput.is_open())
    {
        mErrorCallback(format("Failed to open file {}", mDestination));
        mState = State::Idle;
    }
}

void Client::data()
{
    if (!this->wait(Protocol::Type::Data))
        return;

    lock_guard lock(mMutex);

    auto protocol = static_pointer_cast<DataProtocol>(mProtocols.front());
    mProtocols.pop();

    if (protocol->block() < mNext || mCache.contains(protocol->block()))
    {
        mBlock = protocol->block();
        mState = State::Acknowledge;
        ++mDuplicate;
        return;
    }

    mCache[protocol->block()] = move(protocol->data());
    mBlock                    = protocol->block();

    decltype(mCache)::iterator iter;
    vector<Block>              complete;
    while ((iter = mCache.find(mNext)) != mCache.end())
    {
        auto &[block, data] = *iter;
        mOutput.write(reinterpret_cast<const char *>(data.data()), data.size());
        mChecksum.update(data);
        complete.push_back(block);
        ++mNext;

        if (mProgressCallback)
            mProgressCallback(mSize, static_cast<size_t>(mOutput.tellp()));
    }

    for (auto block : complete)
        mCache.erase(block);

    mState = State::Acknowledge;
}

void Client::acknowledge()
{
    auto protocol = AcknowledgeProtocol(mFile, mBlock);
    mHandle       = mChannel.send({protocol.serialize(), mIP, mPort});
    mState        = (static_cast<size_t>(mOutput.tellp()) == mSize) ? State::Checksum : State::Data;
}

void Client::checksum()
{
    mChecksum.finalize();
    auto protocol = ChecksumProtocol(mFile, mChecksum.digest());
    mHandle       = mChannel.send({protocol.serialize(), mIP, mPort}, mTimeout, mRetries);
    mState        = State::Confirm;
}

void Client::confirm()
{
    if (!this->wait(Protocol::Type::Confirm))
        return;

    if (mFinishCallback)
    {
        auto end = high_resolution_clock::now();
        mFinishCallback(mFile, mSize, duration_cast<milliseconds>(end - mStart));
    }
    mOutput.close();
    mState = State::Idle;
}

void Client::error() noexcept
{
    if (!mErrorCallback)
        return;

    lock_guard lock(mMutex);

    auto protocol = static_pointer_cast<ErrorProtocol>(mProtocols.front());
    mProtocols.pop();
    mErrorCallback(protocol->message());
    mState = State::Idle;
}