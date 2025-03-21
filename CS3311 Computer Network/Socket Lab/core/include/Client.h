#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

#include "Protocol.h"
#include "Socket.h"

class Client
{
public:
    struct Host
    {
        std::string    server;
        int            port   = -1;
        Socket::Domain domain = Socket::Domain::IPv4;

        bool operator==(const Host &other) const { return server == other.server && port == other.port; }
    };

    struct Task
    {
        std::string serverFilePath;
        std::string localFilePath;
    };

    using CallbackType = std::function<void(const Host &, const Task &, const std::string &)>;

public:
    explicit Client();
    virtual ~Client();

    void connect(const Host &host);
    void disconnect(const Host &host);
    void request(const Host &host, const Task &task);
    void onTaskCompleted(CallbackType &&callback);

private:
    struct HostHasher
    {
        inline static const std::hash<std::string> sStringHasher;
        inline static const std::hash<int>         sIntHasher;
        std::size_t operator()(const Host &host) const { return sStringHasher(host.server) ^ sIntHasher(host.port); }
    };

    struct Connection
    {
        Connection(Socket::Domain domain) : socket(domain, Socket::Type::TCP), running(true) {}

        Socket           socket;
        std::queue<Task> tasks;
        std::thread      worker;
        std::string      result;
        bool             running;
    };

    std::unordered_map<Host, std::unique_ptr<Connection>, HostHasher> mConnections;
    std::mutex                                                        mMutex;
    std::thread                                                       mMonitor;
    std::condition_variable                                           mCondition;
    bool                                                              mRunning;

    CallbackType mOnTaskCompleted;

    void monitor();
    void worker(Host host);
};