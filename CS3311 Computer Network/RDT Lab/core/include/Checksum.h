#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <vector>

#include "Exceptions.h"

using Data = std::vector<std::byte>;

class Checksum
{
public:
    explicit Checksum() = default;
    virtual ~Checksum() = default;

    virtual void compute(const Data &data) = 0;
    virtual Data digest() const            = 0;

    virtual bool operator==(const Checksum &other) const noexcept;
    virtual bool operator!=(const Checksum &other) const noexcept;
};

class MD5 : public Checksum
{
public:
    explicit MD5();
    virtual ~MD5() = default;

    void update(const Data &data);
    void finalize();
    void reset();
    void compute(const Data &data) override;
    void compute(std::string_view data);
    void compute(std::istream &file);
    Data digest() const override;

private:
    std::uint32_t                 mA, mB, mC, mD;
    std::array<std::byte, 64>     mBuffer;
    std::array<std::uint32_t, 16> mBlocks;
    std::uint64_t                 mSize;
    bool                          mValid;

    template <typename Iterator>
        requires std::random_access_iterator<Iterator>
    void transform(const Iterator &data);
};

class CRC32 : public Checksum
{
public:
    explicit CRC32();
    virtual ~CRC32() = default;

    void compute(const Data &data) override;
    void compute(const Data::const_iterator &begin, const Data::const_iterator &end);
    void compute(std::string_view data);
    Data digest() const override;

private:
    std::uint32_t mCRC;
    bool          mValid;
};