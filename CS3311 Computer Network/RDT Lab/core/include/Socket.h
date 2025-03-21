#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "Exceptions.h"

using Data = std::vector<std::byte>;

class Socket
{
public:
    enum class Domain { IPv4, IPv6 };
    enum class Type { TCP, UDP, Custom };

public:
    explicit Socket(Domain domain, Type type);
    virtual ~Socket() noexcept;
    Socket(const Socket &)            = delete;
    Socket &operator=(const Socket &) = delete;
    Socket(Socket &&)                 = default;
    Socket &operator=(Socket &&)      = default;

    void                    connect(std::string_view IP, int port);
    void                    bind(int port);
    void                    listen();
    std::unique_ptr<Socket> accept(bool block = true);
    void                    send(const Data &data);
    void                    send(const Data &data, std::string_view IP, int port);
    Data                    receive(bool block = true);
    Data                    receive(std::string &IP, int &port, bool block = true);

    bool isConnected() const noexcept;
    bool isBound() const noexcept;
    bool isListening() const noexcept;

    Domain      getDomain() const noexcept;
    Type        getType() const noexcept;
    std::string getIP() const noexcept;
    int         getPort() const noexcept;

private:
#ifdef _WIN32
    using SocketType = unsigned long long;
#else
    using SocketType = int;
#endif

#ifdef _WIN32
    inline static unsigned   sInstanceCount = 0;
    inline static std::mutex sInstanceMutex = {};
#endif
    inline static constexpr int sBufferSize = 65536;

    SocketType  mSocket;
    Domain      mDomain;
    Type        mType;
    bool        mConnected;
    bool        mBound;
    bool        mListening;
    std::string mIP;
    int         mPort;

    class NonBlocker
    {
    public:
        NonBlocker(SocketType socket, bool block);
        ~NonBlocker();

    private:
        SocketType mSocket;
        bool       mBlock;
    };
};