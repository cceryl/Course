#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "Socket.h"
#include "Timer.h"

class Channel
{
public:
    using Handle = int;

    struct Task
    {
        Data        data;
        std::string IP;
        int         port;
    };

    struct Message
    {
        using Timestamp = std::chrono::high_resolution_clock::time_point;

        std::string IP;
        int         port;
        Data        data;
        Timestamp   send;
        Timestamp   receive;
    };

public:
    explicit Channel(Socket::Domain domain, int port);
    virtual ~Channel() noexcept;

    Handle send(Task task, std::chrono::milliseconds timeout = std::chrono::milliseconds(1), std::size_t retries = 0);
    void   terminate(Handle handle);

    void setRetryCallback(std::function<void(Task)> callback) noexcept;
    void setTimeoutCallback(std::function<void(Task)> callback) noexcept;
    void setReceiveCallback(std::function<void(Message)> callback) noexcept;

    std::size_t corrupted() const noexcept { return mCorrupted; }
    std::size_t lost() const noexcept { return mLost; }
    std::size_t successful() const noexcept { return mSuccessful; }

private:
    struct TaskDetail
    {
        Task                      task;
        std::chrono::milliseconds timeout;
        std::size_t               tries;
        std::size_t               retries;
        Timer                     timer;
    };

    Socket mSocket;

    Handle                                 mHandle;
    std::unordered_map<Handle, TaskDetail> mTasks;
    mutable std::mutex                     mMutex;
    std::function<void(Task)>              mRetryCallback;
    std::function<void(Task)>              mTimeoutCallback;

    std::thread                  mReceiver;
    std::atomic<bool>            mRunning;
    std::function<void(Message)> mReceiveCallback;

    std::size_t mCorrupted;
    std::size_t mLost;
    std::size_t mSuccessful;

    Data encode(const Data &data, std::size_t tries) const;
    auto decode(Data &data) const;
    void sender(Handle handle);
    void receiver() noexcept;
};