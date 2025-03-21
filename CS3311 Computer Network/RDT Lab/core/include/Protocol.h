#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

using Data = std::vector<std::byte>;

class Protocol
{
public:
    enum class Type : std::uint8_t { Unknown, Query, Size, Get, Start, Data, Acknowledge, Checksum, Confirm, Finish, Error, Count };

public:
    explicit Protocol(Type type);
    virtual ~Protocol() = default;

    virtual Data                     serialize() const = 0;
    static std::shared_ptr<Protocol> deserialize(const Data &data);

    virtual const Type &type() const noexcept { return mType; }
    virtual Type       &type() noexcept { return mType; }

protected:
    Type mType = Type::Unknown;

    using Deserializer = std::function<std::shared_ptr<Protocol>(const Data &)>;
    static void registrate(Type type, Deserializer deserializer);

private:
    inline static std::array<Deserializer, static_cast<std::size_t>(Type::Count)> mRegistry;
};

class QueryProtocol : public Protocol
{
public:
    explicit QueryProtocol(std::string_view path);
    virtual ~QueryProtocol() = default;

    virtual Data                          serialize() const override;
    static std::shared_ptr<QueryProtocol> deserialize(const Data &data);

    const std::string &path() const noexcept { return mPath; }
    std::string       &path() noexcept { return mPath; }

private:
    std::string mPath;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Query, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};

class SizeProtocol : public Protocol
{
public:
    explicit SizeProtocol(std::string_view path, std::uint64_t size);
    virtual ~SizeProtocol() = default;

    virtual Data                         serialize() const override;
    static std::shared_ptr<SizeProtocol> deserialize(const Data &data);

    const std::string   &path() const noexcept { return mPath; }
    std::string         &path() noexcept { return mPath; }
    const std::uint64_t &size() const noexcept { return mSize; }
    std::uint64_t       &size() noexcept { return mSize; }

private:
    std::string   mPath;
    std::uint64_t mSize;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Size, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};

class GetProtocol : public Protocol
{
public:
    explicit GetProtocol(std::string_view path);
    virtual ~GetProtocol() = default;

    virtual Data                        serialize() const override;
    static std::shared_ptr<GetProtocol> deserialize(const Data &data);

    const std::string &path() const noexcept { return mPath; }
    std::string       &path() noexcept { return mPath; }

private:
    std::string mPath;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Get, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};

class StartProtocol : public Protocol
{
public:
    explicit StartProtocol(std::string_view path);
    virtual ~StartProtocol() = default;

    virtual Data                          serialize() const override;
    static std::shared_ptr<StartProtocol> deserialize(const Data &data);

    const std::string &path() const noexcept { return mPath; }
    std::string       &path() noexcept { return mPath; }

private:
    std::string mPath;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Start, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};

class DataProtocol : public Protocol
{
public:
    explicit DataProtocol(std::string_view path, std::uint64_t block, Data data);
    virtual ~DataProtocol() = default;

    virtual Data                         serialize() const override;
    static std::shared_ptr<DataProtocol> deserialize(const Data &data);

    const std::string   &path() const noexcept { return mPath; }
    std::string         &path() noexcept { return mPath; }
    const std::uint64_t &block() const noexcept { return mBlock; }
    std::uint64_t       &block() noexcept { return mBlock; }
    const Data          &data() const noexcept { return mData; }
    Data                &data() noexcept { return mData; }

private:
    std::string   mPath;
    std::uint64_t mBlock;
    Data          mData;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Data, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};

class AcknowledgeProtocol : public Protocol
{
public:
    explicit AcknowledgeProtocol(std::string_view path, std::uint64_t block);
    virtual ~AcknowledgeProtocol() = default;

    virtual Data                                serialize() const override;
    static std::shared_ptr<AcknowledgeProtocol> deserialize(const Data &data);

    const std::string   &path() const noexcept { return mPath; }
    std::string         &path() noexcept { return mPath; }
    const std::uint64_t &block() const noexcept { return mBlock; }
    std::uint64_t       &block() noexcept { return mBlock; }

private:
    std::string   mPath;
    std::uint64_t mBlock;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Acknowledge, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};

class ChecksumProtocol : public Protocol
{
public:
    explicit ChecksumProtocol(std::string_view path, const Data &checksum);
    virtual ~ChecksumProtocol() = default;

    virtual Data                             serialize() const override;
    static std::shared_ptr<ChecksumProtocol> deserialize(const Data &data);

    const std::string &path() const noexcept { return mPath; }
    std::string       &path() noexcept { return mPath; }
    const Data        &checksum() const noexcept { return mChecksum; }
    Data              &checksum() noexcept { return mChecksum; }

private:
    std::string mPath;
    Data        mChecksum;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Checksum, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};

class ConfirmProtocol : public Protocol
{
public:
    explicit ConfirmProtocol(std::string_view path);
    virtual ~ConfirmProtocol() = default;

    virtual Data                            serialize() const override;
    static std::shared_ptr<ConfirmProtocol> deserialize(const Data &data);

    const std::string &path() const noexcept { return mPath; }
    std::string       &path() noexcept { return mPath; }

private:
    std::string mPath;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Confirm, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};

class ErrorProtocol : public Protocol
{
public:
    explicit ErrorProtocol(std::string_view path, std::string_view message);
    virtual ~ErrorProtocol() = default;

    virtual Data                          serialize() const override;
    static std::shared_ptr<ErrorProtocol> deserialize(const Data &data);

    const std::string &path() const noexcept { return mPath; }
    std::string       &path() noexcept { return mPath; }
    const std::string &message() const noexcept { return mMessage; }
    std::string       &message() noexcept { return mMessage; }

private:
    std::string mPath;
    std::string mMessage;

    /* clang-format off */
    struct Registrar { Registrar() { Protocol::registrate(Type::Error, deserialize); } };
    inline static Registrar registrar;
    /* clang-format on */
};