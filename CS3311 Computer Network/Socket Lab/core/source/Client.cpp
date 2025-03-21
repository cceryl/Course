#include "Client.h"

#include <format>
#include <fstream>

using std::condition_variable, std::unique_lock;
using std::ios, std::ofstream, std::exception, std::format;
using std::string, std::function, std::move;
using std::thread, std::mutex, std::lock_guard;
using std::unique_ptr, std::make_unique;

Client::Client()
{
    mRunning = true;
    mMonitor = thread(&Client::monitor, this);
}

Client::~Client()
{
    {
        lock_guard<mutex> lock(mMutex);

        for (auto &[host, connection] : mConnections)
        {
            connection->running = false;
            if (connection->worker.joinable())
                connection->worker.join();
        }
    }

    mRunning = false;
    mCondition.notify_one();
    if (mMonitor.joinable())
        mMonitor.join();
}

void Client::connect(const Host &host)
{
    lock_guard<mutex> lock(mMutex);

    if (mConnections.contains(host))
        throw ConnectionError("Connection already exists");

    mConnections.emplace(host, make_unique<Connection>(host.domain));
    try
    {
        mConnections.at(host)->socket.connect(host.server, host.port);
    }
    catch (const exception &e)
    {
        mConnections.erase(host);
        throw;
    }
}

void Client::disconnect(const Host &host)
{
    lock_guard<mutex> lock(mMutex);

    if (!mConnections.contains(host))
        throw ConnectionError("Connection does not exist");

    mConnections.at(host)->running = false;
    if (mConnections.at(host)->worker.joinable())
        mConnections.at(host)->worker.join();
    mConnections.erase(host);
}

void Client::request(const Host &host, const Task &task)
{
    lock_guard<mutex> lock(mMutex);

    if (!mConnections.contains(host))
        throw ConnectionError("Connection does not exist");

    mConnections.at(host)->tasks.push(task);
    mCondition.notify_one();
}

void Client::onTaskCompleted(CallbackType &&callback) { mOnTaskCompleted = move(callback); }

void Client::monitor()
{
    while (mRunning)
    {
        unique_lock<mutex> lock(mMutex);
        mCondition.wait(lock);

        for (auto &[host, connection] : mConnections)
        {
            if (connection->worker.joinable() && !connection->result.empty())
            {
                if (mOnTaskCompleted)
                    mOnTaskCompleted(host, connection->tasks.front(), connection->result);
                connection->tasks.pop();
                connection->result.clear();
                connection->worker.join();
            }

            if (!connection->worker.joinable() && !connection->tasks.empty() && connection->running)
                connection->worker = thread(&Client::worker, this, host);
        }
    }
}

void Client::worker(Host host)
{
    auto connection = mConnections.at(host).get();
    Data buffer;

    try
    {
        const Task &task    = connection->tasks.front();
        auto        request = GetProtocol(task.serverFilePath).serialize();
        connection->socket.send(request);

        while (connection->running)
        {
            auto response = connection->socket.receive(false);
            if (response.empty())
                continue;

            buffer.insert(buffer.end(), response.begin(), response.end());
            auto distilled = Protocol::distill(buffer);
            if (distilled.empty())
                continue;

            auto message = Protocol::deserialize(distilled);
            switch (message->type())
            {
            case Protocol::Type::File:
            {
                auto     file  = dynamic_cast<FileProtocol *>(message.get());
                ofstream local = ofstream(task.localFilePath, ios::binary);
                if (!local)
                    throw SystemError(format("Failed to open file: {}", task.localFilePath));
                local.write(reinterpret_cast<const char *>(file->data().data()), file->data().size());
                connection->result = "Success";
                break;
            }
            case Protocol::Type::Error:
            {
                auto error         = dynamic_cast<ErrorProtocol *>(message.get());
                connection->result = error->message();
                break;
            }
            default: connection->result = "Invalid response type";
            }

            mCondition.notify_one();
            break;
        }
    }
    catch (const exception &e)
    {
        connection->result = e.what();
        mCondition.notify_one();
    }
}