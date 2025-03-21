#pragma once

#include <exception>
#include <format>

class WSAError : public std::exception
{
public:
    WSAError(std::string_view message);
    const char *what() const noexcept override;

private:
    std::string mMessage;
};

class SocketError : public std::exception
{
public:
    SocketError(std::string_view message);
    const char *what() const noexcept override;

private:
    std::string mMessage;
};

class ConnectionError : public std::exception
{
public:
    ConnectionError(std::string_view message);
    const char *what() const noexcept override;

private:
    std::string mMessage;
};

class ProtocolError : public std::exception
{
public:
    ProtocolError(std::string_view message);
    const char *what() const noexcept override;

private:
    std::string mMessage;
};

class SystemError : public std::exception
{
public:
    SystemError(std::string_view message);
    const char *what() const noexcept override;

private:
    std::string mMessage;
};

class ChecksumError : public std::exception
{
public:
    ChecksumError(std::string_view message);
    const char *what() const noexcept override;

private:
    std::string mMessage;
};
