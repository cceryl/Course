#include "Socket.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
using AddressSize = int;
#else
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <unistd.h>
using AddressSize = socklen_t;
#endif

static std::string getErrorMessage()
{
    char message[512];
#ifdef _WIN32
    int error = ::WSAGetLastError();
    ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, message, sizeof(message), nullptr);
#else
    [[maybe_unused]] char *ret = ::strerror_r(errno, message, sizeof(message));
#endif
    return message;
}

[[maybe_unused]] static std::string getErrorMessage(int error)
{
    char message[512];
#ifdef _WIN32
    ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, message, sizeof(message), nullptr);
#else
    [[maybe_unused]] char *ret = ::strerror_r(error, message, sizeof(message));
#endif
    return message;
}

Socket::Socket(Domain domain, Type type)
{
#ifdef _WIN32
    {
        std::lock_guard<std::mutex> lock(sInstanceMutex);

        WSAData wsaData;
        if (sInstanceCount == 0 && ::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            throw WSAError(getErrorMessage());

        ++sInstanceCount;
    }
#endif

    int domainCode = domain == Domain::IPv4 ? AF_INET : AF_INET6;
    int typeCode   = type == Type::TCP ? SOCK_STREAM : SOCK_DGRAM;
    int protocol   = type == Type::TCP ? IPPROTO_TCP : IPPROTO_UDP;
    mSocket        = ::socket(domainCode, typeCode, protocol);

#ifdef _WIN32
    #pragma message("SO_NO_CHECK is not supported on Windows")
#else
    if (type == Type::Custom)
    {
        int yes = 1;
        if (::setsockopt(mSocket, SOL_SOCKET, SO_NO_CHECK, (void *)&yes, sizeof(yes)) != 0)
            throw SocketError(getErrorMessage());
    }
#endif

#ifdef _WIN32
    if (mSocket == INVALID_SOCKET)
#else
    if (mSocket == -1)
#endif
    {
        this->~Socket();
        throw SocketError(getErrorMessage());
    }

    mDomain    = domain;
    mType      = type;
    mConnected = false;
    mBound     = false;
    mListening = false;
    mIP        = "";
    mPort      = 0;
}

Socket::~Socket()
{
#ifdef _WIN32
    {
        std::lock_guard<std::mutex> lock(sInstanceMutex);

        --sInstanceCount;
        if (sInstanceCount == 0)
            ::WSACleanup();
    }
#endif

#ifdef _WIN32
    ::closesocket(mSocket);
#else
    ::close(mSocket);
#endif
}

void Socket::connect(std::string_view IP, int port)
{
    if (mDomain == Domain::IPv4)
    {
        sockaddr_in address;
        std::fill_n(reinterpret_cast<char *>(&address), sizeof(address), 0);
        address.sin_family = AF_INET;
        address.sin_port   = ::htons(port);

        if (::inet_pton(AF_INET, IP.data(), &address.sin_addr) != 1)
            throw ConnectionError(std::format("Invalid IP address {}", IP));

        if (::connect(mSocket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1)
            throw ConnectionError(getErrorMessage());
    }

    if (mDomain == Domain::IPv6)
    {
        sockaddr_in6 address;
        std::fill_n(reinterpret_cast<char *>(&address), sizeof(address), 0);
        address.sin6_family = AF_INET6;
        address.sin6_port   = ::htons(port);

        if (::inet_pton(AF_INET6, IP.data(), &address.sin6_addr) != 1)
            throw ConnectionError(std::format("Invalid IP address {}", IP));

        if (::connect(mSocket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1)
            throw ConnectionError(getErrorMessage());
    }

    mConnected = true;
    mIP        = IP;
    mPort      = port;
}

void Socket::bind(int port)
{
    if (mDomain == Domain::IPv4)
    {
        sockaddr_in address;
        std::fill_n(reinterpret_cast<char *>(&address), sizeof(address), 0);
        address.sin_family      = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port        = ::htons(port);

        if (::bind(mSocket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1)
            throw ConnectionError(getErrorMessage());
    }

    if (mDomain == Domain::IPv6)
    {
        sockaddr_in6 address;
        std::fill_n(reinterpret_cast<char *>(&address), sizeof(address), 0);
        address.sin6_family = AF_INET6;
        address.sin6_addr   = in6addr_any;
        address.sin6_port   = ::htons(port);

        if (::bind(mSocket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1)
            throw ConnectionError(getErrorMessage());
    }

    mBound = true;
    mPort  = port;
}

void Socket::listen()
{
    if (::listen(mSocket, SOMAXCONN) == -1)
        throw ConnectionError(getErrorMessage());

    mListening = true;
}

std::unique_ptr<Socket> Socket::accept(bool block)
{
    NonBlocker blocker(mSocket, block);

    SocketType  clientSocket = -1;
    std::string clientIP;
    int         clientPort = -1;

    if (mDomain == Domain::IPv4)
    {
        sockaddr_in address;
        std::fill_n(reinterpret_cast<char *>(&address), sizeof(address), 0);
        AddressSize size = sizeof(address);
        clientSocket     = ::accept(mSocket, reinterpret_cast<sockaddr *>(&address), &size);

        char buffer[INET_ADDRSTRLEN];
        ::inet_ntop(AF_INET, &address.sin_addr, buffer, sizeof(buffer));
        clientIP   = buffer;
        clientPort = ::ntohs(address.sin_port);
    }

    if (mDomain == Domain::IPv6)
    {
        sockaddr_in6 address;
        std::fill_n(reinterpret_cast<char *>(&address), sizeof(address), 0);
        AddressSize size = sizeof(address);
        clientSocket     = ::accept(mSocket, reinterpret_cast<sockaddr *>(&address), &size);

        char buffer[INET6_ADDRSTRLEN];
        ::inet_ntop(AF_INET6, &address.sin6_addr, buffer, sizeof(buffer));
        clientIP   = buffer;
        clientPort = ::ntohs(address.sin6_port);
    }

#ifdef _WIN32
    if (clientSocket == INVALID_SOCKET)
    {
        int error = ::WSAGetLastError();
        if (error == WSAEWOULDBLOCK)
            return std::make_unique<Socket>(mDomain, mType);

        throw ConnectionError(getErrorMessage(error));
    }
#else
    if (clientSocket == -1)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return std::make_unique<Socket>(mDomain, mType);

        throw ConnectionError(getErrorMessage());
    }
#endif

    std::unique_ptr<Socket> client = std::make_unique<Socket>(mDomain, mType);
    client->mSocket                = clientSocket;
    client->mConnected             = true;
    client->mIP                    = clientIP;
    client->mPort                  = clientPort;
    return client;
}

void Socket::send(const Data &data)
{
    if (::send(mSocket, reinterpret_cast<const char *>(data.data()), data.size(), 0) != static_cast<int>(data.size()))
        throw ConnectionError(getErrorMessage());
}

void Socket::send(const Data &data, std::string_view IP, int port)
{
    if (mDomain == Domain::IPv4)
    {
        sockaddr_in address;
        std::fill_n(reinterpret_cast<char *>(&address), sizeof(address), 0);
        address.sin_family = AF_INET;
        address.sin_port   = ::htons(port);

        if (::inet_pton(AF_INET, IP.data(), &address.sin_addr) != 1)
            throw ConnectionError(std::format("Invalid IP address {}", IP));

        auto raw  = reinterpret_cast<const char *>(data.data());
        int  size = static_cast<int>(data.size());
        if (::sendto(mSocket, raw, size, 0, reinterpret_cast<sockaddr *>(&address), sizeof(address)) != size)
            throw ConnectionError(getErrorMessage());
    }

    if (mDomain == Domain::IPv6)
    {
        sockaddr_in6 address;
        std::fill_n(reinterpret_cast<char *>(&address), sizeof(address), 0);
        address.sin6_family = AF_INET6;
        address.sin6_port   = ::htons(port);

        if (::inet_pton(AF_INET6, IP.data(), &address.sin6_addr) != 1)
            throw ConnectionError(std::format("Invalid IP address {}", IP));

        auto raw  = reinterpret_cast<const char *>(data.data());
        int  size = static_cast<int>(data.size());
        if (::sendto(mSocket, raw, size, 0, reinterpret_cast<sockaddr *>(&address), sizeof(address)) != size)
            throw ConnectionError(getErrorMessage());
    }
}

Data Socket::receive(bool block)
{
    NonBlocker blocker(mSocket, block);

    std::byte buffer[Socket::sBufferSize];
    int       size = ::recv(mSocket, reinterpret_cast<char *>(buffer), sizeof(buffer), 0);

    if (size < 0)
    {
#ifdef _WIN32
        if (::WSAGetLastError() == WSAEWOULDBLOCK)
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN)
#endif
            return Data();

        throw ConnectionError(getErrorMessage());
    }

    if (size == 0)
        throw ConnectionError("Connection terminated by peer");

    return Data(buffer, buffer + size);
}

Data Socket::receive(std::string &IP, int &port, bool block)
{
    NonBlocker blocker(mSocket, block);

    std::byte buffer[Socket::sBufferSize];
    int       size = -1;

    if (mDomain == Domain::IPv4)
    {
        sockaddr_in address;

        AddressSize addrSize = sizeof(address);
        size = ::recvfrom(mSocket, reinterpret_cast<char *>(buffer), sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&address), &addrSize);

        char ip[INET_ADDRSTRLEN];
        ::inet_ntop(AF_INET, &address.sin_addr, ip, sizeof(ip));
        IP   = ip;
        port = ::ntohs(address.sin_port);
    }

    if (mDomain == Domain::IPv6)
    {
        sockaddr_in6 address;

        AddressSize addrSize = sizeof(address);
        size = ::recvfrom(mSocket, reinterpret_cast<char *>(buffer), sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&address), &addrSize);

        char ip[INET6_ADDRSTRLEN];
        ::inet_ntop(AF_INET6, &address.sin6_addr, ip, sizeof(ip));
        IP   = ip;
        port = ::ntohs(address.sin6_port);
    }

    if (size < 0)
    {
#ifdef _WIN32
        if (::WSAGetLastError() == WSAEWOULDBLOCK)
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN)
#endif
            return Data();

        throw ConnectionError(getErrorMessage());
    }

    if (size == 0)
        throw ConnectionError("Connection terminated by peer");

    return Data(buffer, buffer + size);
}

bool Socket::isConnected() const noexcept { return mConnected; }

bool Socket::isBound() const noexcept { return mBound; }

bool Socket::isListening() const noexcept { return mListening; }

Socket::Domain Socket::getDomain() const noexcept { return mDomain; }

Socket::Type Socket::getType() const noexcept { return mType; }

std::string Socket::getIP() const noexcept { return mIP; }

int Socket::getPort() const noexcept { return mPort; }

Socket::NonBlocker::NonBlocker(SocketType socket, bool block)
{
    mSocket = socket;
    mBlock  = block;

    if (block)
        return;

#ifdef _WIN32
    u_long mode = 1;
    if (::ioctlsocket(mSocket, FIONBIO, &mode) == SOCKET_ERROR)
        throw SocketError(getErrorMessage());
#else
    int flags = ::fcntl(mSocket, F_GETFL, 0);
    if (flags == -1)
        throw SocketError(getErrorMessage());

    flags |= O_NONBLOCK;

    if (::fcntl(mSocket, F_SETFL, flags) == -1)
        throw SocketError(getErrorMessage());
#endif
}

Socket::NonBlocker::~NonBlocker()
{
    if (mBlock)
        return;

#ifdef _WIN32
    u_long mode = 0;
    ::ioctlsocket(mSocket, FIONBIO, &mode);
#else
    int flags  = ::fcntl(mSocket, F_GETFL, 0);
    flags     &= ~O_NONBLOCK;
    ::fcntl(mSocket, F_SETFL, flags);
#endif
}
