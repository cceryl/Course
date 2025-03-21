#include "Protocol.h"

#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>

#include "Exceptions.h"

using Type = Protocol::Type;
using std::copy, std::back_inserter;
using std::distance, std::advance;
using std::is_default_constructible_v;
using std::is_same_v;
using std::is_trivially_copyable_v;
using std::shared_ptr, std::make_shared;
using std::string, std::string_view;
using std::uint8_t, std::uint64_t;
using std::vector, std::byte;

template <typename T>
    requires is_trivially_copyable_v<T> && is_default_constructible_v<T>
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
    requires is_trivially_copyable_v<T> && is_default_constructible_v<T>
T read(const Data &data, Data::const_iterator &it)
{
    if (distance(it, data.cend()) < static_cast<int>(sizeof(T)))
        throw ProtocolError("Invalid data size");

    T value;

    copy(it, it + sizeof(value), reinterpret_cast<byte *>(&value));

    advance(it, sizeof(value));
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

shared_ptr<Protocol> Protocol::deserialize(const Data &data)
{
    Data::const_iterator it = data.cbegin();

    uint64_t size = read<uint64_t>(data, it);
    if (data.size() != sizeof(uint64_t) + size)
        throw ProtocolError("Invalid data size");

    Type type = static_cast<Type>(read<uint8_t>(data, it));
    switch (type)
    {
    case Type::Get:   return make_shared<GetProtocol>(read<string>(data, it));
    case Type::File:  return make_shared<FileProtocol>(read<Data>(data, it));
    case Type::Error: return make_shared<ErrorProtocol>(read<string>(data, it));
    default:          throw ProtocolError("Invalid protocol type");
    }
}

Data Protocol::distill(Data &data)
{
    if (data.size() < sizeof(uint64_t))
        return {};

    Data::const_iterator it   = data.cbegin();
    uint64_t             size = read<uint64_t>(data, it);
    if (data.size() < sizeof(uint64_t) + size)
        return {};

    Data distilled(data.cbegin(), data.cbegin() + sizeof(uint64_t) + size);
    data.erase(data.begin(), data.begin() + sizeof(uint64_t) + size);

    return distilled;
}

const Type &Protocol::type() const noexcept { return mType; }

Type &Protocol::type() noexcept { return mType; }

GetProtocol::GetProtocol(string_view path) : mPath(path) { mType = Type::Get; }

Data GetProtocol::serialize()
{
    Data     data;
    uint64_t size = sizeof(Type) + mPath.size() + 1;
    data.reserve(sizeof(size) + size);

    write(data, size);
    write(data, static_cast<uint8_t>(Type::Get));
    write(data, mPath);

    return data;
}

const string &GetProtocol::path() const noexcept { return mPath; }

string &GetProtocol::path() noexcept { return mPath; }

FileProtocol::FileProtocol(const Data &data) : mData(data) { mType = Type::File; }

Data FileProtocol::serialize()
{
    Data     data;
    uint64_t size = sizeof(Type) + mData.size();
    data.reserve(sizeof(size) + size);

    write(data, size);
    write(data, static_cast<uint8_t>(Type::File));
    write(data, mData);

    return data;
}

const Data &FileProtocol::data() const noexcept { return mData; }

Data &FileProtocol::data() noexcept { return mData; }

ErrorProtocol::ErrorProtocol(string_view message) : mMessage(message) { mType = Type::Error; }

Data ErrorProtocol::serialize()
{
    Data     data;
    uint64_t size = sizeof(Type) + mMessage.size() + 1;
    data.reserve(sizeof(size) + size);

    write(data, size);
    write(data, static_cast<uint8_t>(Type::Error));
    write(data, mMessage);

    return data;
}

const string &ErrorProtocol::message() const noexcept { return mMessage; }

string &ErrorProtocol::message() noexcept { return mMessage; }
