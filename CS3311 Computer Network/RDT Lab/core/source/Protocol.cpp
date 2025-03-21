#include "Protocol.h"

#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>

#include "Exceptions.h"

using Type = Protocol::Type;
using std::copy, std::back_inserter, std::distance, std::advance;
using std::is_same_v, std::is_trivial_v, std::is_default_constructible_v, std::is_standard_layout_v;
using std::shared_ptr, std::make_shared;
using std::string, std::string_view;
using std::vector, std::byte, std::uint8_t, std::uint64_t;

template <typename T>
    requires is_trivial_v<T> && is_standard_layout_v<T> && is_default_constructible_v<T>
void write(Data &data, const T &value)
{
    const byte *bytes = reinterpret_cast<const byte *>(&value);
    data.insert(data.end(), bytes, bytes + sizeof(value));
}

template <typename T>
    requires is_same_v<T, string>
void write(Data &data, const T &string)
{
    const byte *bytes = reinterpret_cast<const byte *>(string.data());
    data.insert(data.end(), bytes, bytes + string.size());
    data.push_back(byte{'\0'});
}

template <typename T>
    requires is_same_v<T, Data>
void write(Data &data, const T &bytes)
{
    data.insert(data.end(), bytes.begin(), bytes.end());
}

template <typename T>
void write(Data &data, const T &value)
{
    static_assert(false, "Unsupported type");
}

template <typename T>
    requires is_trivial_v<T> && is_standard_layout_v<T> && is_default_constructible_v<T>
T read(const Data &data, Data::const_iterator &it)
{
    if (distance(it, data.cend()) < static_cast<int>(sizeof(T)))
        throw ProtocolError("Invalid data size");

    T value;

    copy(it, it + sizeof(T), reinterpret_cast<byte *>(&value));

    advance(it, sizeof(T));
    return value;
}

template <typename T>
    requires is_same_v<T, string>
T read(const Data &data, Data::const_iterator &it)
{
    T string;

    while (*it != byte{'\0'})
    {
        if (it == data.cend())
            throw ProtocolError("Invalid data size");

        string += static_cast<char>(*it);
        advance(it, 1);
    }

    advance(it, 1);
    return string;
}

template <typename T>
    requires is_same_v<T, Data>
T read(const Data &data, Data::const_iterator &it)
{
    T bytes;

    bytes.reserve(distance(it, data.cend()));
    copy(it, data.cend(), back_inserter(bytes));

    return bytes;
}

template <typename T>
T read(const Data &data, Data::const_iterator &it)
{
    static_assert(false, "Unsupported type");
}

Protocol::Protocol(Type type) : mType(type) {}

shared_ptr<Protocol> Protocol::deserialize(const Data &data)
{
    auto it   = data.cbegin();
    Type type = read<Type>(data, it);

    if (mRegistry[static_cast<size_t>(type)] == nullptr)
        throw ProtocolError("Unknown protocol type");

    return mRegistry[static_cast<size_t>(type)](data);
}

void Protocol::registrate(Type type, Deserializer deserializer) { mRegistry[static_cast<size_t>(type)] = deserializer; }

QueryProtocol::QueryProtocol(string_view path) : Protocol(Type::Query), mPath(path) {}

Data QueryProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1);

    write(data, mType);
    write(data, mPath);

    return data;
}

shared_ptr<QueryProtocol> QueryProtocol::deserialize(const Data &data)
{
    auto it   = data.cbegin() + sizeof(Type);
    auto path = read<string>(data, it);
    return make_shared<QueryProtocol>(path);
}

SizeProtocol::SizeProtocol(string_view path, uint64_t size) : Protocol(Type::Size), mPath(path), mSize(size) {}

Data SizeProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1 + sizeof(uint64_t));

    write(data, mType);
    write(data, mPath);
    write(data, mSize);

    return data;
}

shared_ptr<SizeProtocol> SizeProtocol::deserialize(const Data &data)
{
    auto it   = data.cbegin() + sizeof(Type);
    auto path = read<string>(data, it);
    auto size = read<uint64_t>(data, it);
    return make_shared<SizeProtocol>(path, size);
}

GetProtocol::GetProtocol(string_view path) : Protocol(Type::Get), mPath(path) {}

Data GetProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1);

    write(data, mType);
    write(data, mPath);

    return data;
}

shared_ptr<GetProtocol> GetProtocol::deserialize(const Data &data)
{
    auto it   = data.cbegin() + sizeof(Type);
    auto path = read<string>(data, it);
    return make_shared<GetProtocol>(path);
}

StartProtocol::StartProtocol(string_view path) : Protocol(Type::Start), mPath(path) {}

Data StartProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1);

    write(data, mType);
    write(data, mPath);

    return data;
}

shared_ptr<StartProtocol> StartProtocol::deserialize(const Data &data)
{
    auto it   = data.cbegin() + sizeof(Type);
    auto path = read<string>(data, it);
    return make_shared<StartProtocol>(path);
}

DataProtocol::DataProtocol(string_view path, uint64_t block, Data data) : Protocol(Type::Data), mPath(path), mBlock(block), mData(move(data)) {}

Data DataProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1 + sizeof(uint64_t) + mData.size());

    write(data, mType);
    write(data, mPath);
    write(data, mBlock);
    write(data, mData);

    return data;
}

shared_ptr<DataProtocol> DataProtocol::deserialize(const Data &data)
{
    auto it    = data.cbegin() + sizeof(Type);
    auto path  = read<string>(data, it);
    auto block = read<uint64_t>(data, it);
    auto bytes = read<Data>(data, it);
    return make_shared<DataProtocol>(path, block, bytes);
}

AcknowledgeProtocol::AcknowledgeProtocol(string_view path, uint64_t block) : Protocol(Type::Acknowledge), mPath(path), mBlock(block) {}

Data AcknowledgeProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1 + sizeof(uint64_t));

    write(data, mType);
    write(data, mPath);
    write(data, mBlock);

    return data;
}

shared_ptr<AcknowledgeProtocol> AcknowledgeProtocol::deserialize(const Data &data)
{
    auto it    = data.cbegin() + sizeof(Type);
    auto path  = read<string>(data, it);
    auto block = read<uint64_t>(data, it);
    return make_shared<AcknowledgeProtocol>(path, block);
}

ChecksumProtocol::ChecksumProtocol(string_view path, const Data &checksum) : Protocol(Type::Checksum), mPath(path), mChecksum(checksum) {}

Data ChecksumProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1 + mChecksum.size());

    write(data, mType);
    write(data, mPath);
    write(data, mChecksum);

    return data;
}

shared_ptr<ChecksumProtocol> ChecksumProtocol::deserialize(const Data &data)
{
    auto it    = data.cbegin() + sizeof(Type);
    auto path  = read<string>(data, it);
    auto bytes = read<Data>(data, it);
    return make_shared<ChecksumProtocol>(path, bytes);
}

ConfirmProtocol::ConfirmProtocol(string_view path) : Protocol(Type::Confirm), mPath(path) {}

Data ConfirmProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1);

    write(data, mType);
    write(data, mPath);

    return data;
}

shared_ptr<ConfirmProtocol> ConfirmProtocol::deserialize(const Data &data)
{
    auto it   = data.cbegin() + sizeof(Type);
    auto path = read<string>(data, it);
    return make_shared<ConfirmProtocol>(path);
}

ErrorProtocol::ErrorProtocol(string_view path, string_view message) : Protocol(Type::Error), mPath(path), mMessage(message) {}

Data ErrorProtocol::serialize() const
{
    Data data;
    data.reserve(sizeof(Type) + mPath.length() + 1 + mMessage.length() + 1);

    write(data, mType);
    write(data, mPath);
    write(data, mMessage);

    return data;
}

shared_ptr<ErrorProtocol> ErrorProtocol::deserialize(const Data &data)
{
    auto it      = data.cbegin() + sizeof(Type);
    auto path    = read<string>(data, it);
    auto message = read<string>(data, it);
    return make_shared<ErrorProtocol>(path, message);
}