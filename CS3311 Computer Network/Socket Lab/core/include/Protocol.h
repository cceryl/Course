#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Socket.h"

class Protocol
{
public:
    enum class Type : unsigned char { Unknown, Get, File, Error };

public:
    virtual ~Protocol() = default;

    virtual Data                     serialize() = 0;
    static std::shared_ptr<Protocol> deserialize(const Data &data);
    static Data                      distill(Data &data);

    virtual const Type &type() const noexcept;
    virtual Type       &type() noexcept;

protected:
    Type mType = Type::Unknown;
};

class GetProtocol : public Protocol
{
public:
    explicit GetProtocol(std::string_view path);
    virtual ~GetProtocol() = default;

    virtual Data serialize() override;

    const std::string &path() const noexcept;
    std::string       &path() noexcept;

private:
    std::string mPath;
};

class FileProtocol : public Protocol
{
public:
    explicit FileProtocol(const Data &data);
    virtual ~FileProtocol() = default;

    virtual Data serialize() override;

    const Data &data() const noexcept;
    Data       &data() noexcept;

private:
    Data mData;
};

class ErrorProtocol : public Protocol
{
public:
    explicit ErrorProtocol(std::string_view message);
    virtual ~ErrorProtocol() = default;

    virtual Data serialize() override;

    const std::string &message() const noexcept;
    std::string       &message() noexcept;

private:
    std::string mMessage;
};