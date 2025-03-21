#pragma once

#include <condition_variable>
#include <fstream>

#include "Channel.h"
#include "Checksum.h"
#include "Protocol.h"

class Server
{
public:
    struct Client
    {
        std::string IP;
        int         port;

        bool operator==(const Client &other) const noexcept { return IP == other.IP && port == other.port; }

        struct Hash
        {
            std::size_t operator()(const Client &client) const noexcept
            {
                return std::hash<std::string>{}(client.IP) ^ std::hash<int>{}(client.port);
            }
        };
    };

    using TransferCallback = std::function<void(Client, std::string_view)>;
    using ErrorCallback    = std::function<void(const std::string &)>;

public:
    explicit Server(Socket::Domain domain, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000), std::size_t retries = 5);
    virtual ~Server();

    void setTaskCallback(TransferCallback callback) noexcept;
    void setFinishCallback(TransferCallback callback) noexcept;
    void setErrorCallback(ErrorCallback callback) noexcept;

    void start();
    void stop();

private:
    struct Task
    {
        using Block = std::size_t;
        struct Packet
        {
            Channel::Handle             handle;
            Channel::Message::Timestamp departure;
        };

        std::string                       path;
        std::uint64_t                     size;
        std::ifstream                     file;
        MD5                               checksum;
        std::unordered_map<Block, Packet> packets;
        std::size_t                       window;
        std::chrono::milliseconds         average;
        std::chrono::milliseconds         variance;
        std::chrono::milliseconds         timeout;

        Task(std::string path, std::uint64_t size, std::ifstream file, std::size_t window, std::chrono::milliseconds timeout) :
            path(std::move(path)),
            size(size),
            file(std::move(file)),
            window(window),
            average(timeout),
            variance(std::chrono::milliseconds(0)),
            timeout(timeout)
        {}
    };

    inline static constexpr std::size_t sBlockSize = 1024;
    inline static constexpr std::size_t sMaxWindow = 32;
    inline static constexpr std::size_t sSlowStart = 16;

    std::chrono::milliseconds mTimeout;
    std::size_t               mRetries;

    Channel mChannel;
    bool    mRunning;

    std::thread                                    mThread;
    std::condition_variable                        mCondition;
    std::mutex                                     mMutex;
    std::unordered_map<Client, Task, Client::Hash> mTasks;
    std::unordered_map<std::string, Data>          mMD5Cache;

    TransferCallback mTaskCallback;
    TransferCallback mFinishCallback;
    ErrorCallback    mErrorCallback;

    void worker() noexcept;
    void terminate(Client client);

    void onRetry(Channel::Task task);
    void onTimeout(Channel::Task task);
    void onReceive(Channel::Message message);

    void onQuery(Client client, std::shared_ptr<QueryProtocol> protocol);
    void onGet(Client client, std::shared_ptr<GetProtocol> protocol);
    void onAcknowledge(Client client, std::shared_ptr<AcknowledgeProtocol> protocol);
    void onChecksum(Client client, std::shared_ptr<ChecksumProtocol> protocol);
};