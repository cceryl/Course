#pragma once

#include <condition_variable>
#include <fstream>
#include <functional>
#include <map>
#include <queue>
#include <chrono>

#include "Channel.h"
#include "Checksum.h"
#include "Protocol.h"

class Client
{
public:
    using ProgressCallback = std::function<void(std::size_t, std::size_t)>;
    using FinishCallback   = std::function<void(std::string_view, std::size_t, std::chrono::milliseconds)>;
    using ErrorCallback    = std::function<void(const std::string &)>;

    struct Stats
    {
        size_t corrupted;
        size_t duplicate;
        size_t lost;
        size_t successful;
    };

public:
    explicit Client(Socket::Domain domain, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000), std::size_t retries = 5);
    virtual ~Client() noexcept = default;

    void setProgressCallback(ProgressCallback callback) noexcept;
    void setFinishCallback(FinishCallback callback) noexcept;
    void setErrorCallback(ErrorCallback callback) noexcept;

    void  download(std::string_view IP, int port, std::string_view file, std::string_view destination);
    Stats statistics() const noexcept;

private:
    enum class State { Idle, Query, Size, Get, Start, Data, Acknowledge, Checksum, Confirm, Error, Count };
    using Block     = std::size_t;
    using Timastamp = std::chrono::high_resolution_clock::time_point;

    std::chrono::milliseconds mTimeout;
    std::size_t               mRetries;

    std::atomic<State>                    mState;
    Channel                               mChannel;
    Channel::Handle                       mHandle;
    std::queue<std::shared_ptr<Protocol>> mProtocols;
    std::mutex                            mMutex;
    std::condition_variable               mCondition;

    std::string           mIP;
    int                   mPort;
    std::string           mFile;
    std::string           mDestination;
    Timastamp             mStart;
    std::size_t           mSize;
    std::ofstream         mOutput;
    std::size_t           mBlock;
    std::size_t           mNext;
    std::map<Block, Data> mCache;
    MD5                   mChecksum;
    std::size_t           mDuplicate;

    ProgressCallback mProgressCallback;
    FinishCallback   mFinishCallback;
    ErrorCallback    mErrorCallback;

    void dispatch();
    bool wait(Protocol::Type type);
    void onTimeout(Channel::Task task);
    void onReceive(Channel::Message message);

    void query();
    void size();
    void get();
    void start();
    void data();
    void acknowledge();
    void checksum();
    void confirm();
    void error() noexcept;
};