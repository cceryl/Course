#include "Checksum.h"

#include <algorithm>
#include <functional>

using std::function, std::swap, std::copy, std::fill_n;
using std::random_access_iterator, std::distance;
using std::string, std::string_view;
using std::transform, std::back_inserter, std::istream, std::ptrdiff_t;
using std::vector, std::array, std::byte, std::uint32_t, std::uint64_t;

bool Checksum::operator==(const Checksum &other) const noexcept
{
    /* clang-format off */
    try { return this->digest() == other.digest(); }
    catch (const ChecksumError &) { return false; }
    /* clang-format on */
}

bool Checksum::operator!=(const Checksum &other) const noexcept { return !(*this == other); }

MD5::MD5() { this->reset(); }

void MD5::update(const Data &data)
{
    if (mValid)
        throw ChecksumError("MD5: cannot update finalized checksum");

    int bufferSize = mSize % 64;

    if (bufferSize + data.size() < 64)
    {
        copy(data.begin(), data.end(), mBuffer.begin() + bufferSize);
        mSize += data.size();
        return;
    }

    auto iter = data.cbegin();

    if (bufferSize != 0)
    {
        copy(data.begin(), data.begin() + 64 - bufferSize, mBuffer.begin() + bufferSize);
        this->transform(mBuffer.cbegin());
        iter += 64 - bufferSize;
    }

    while (distance(iter, data.cend()) >= 64)
    {
        this->transform(iter);
        iter += 64;
    }

    copy(data.cbegin() + (data.size() - (data.size() % 64)), data.cend(), mBuffer.begin());

    mSize += data.size();
}

void MD5::finalize()
{
    int bufferSize        = mSize % 64;
    mBuffer[bufferSize++] = static_cast<byte>(0x80);

    if (bufferSize > 56)
    {
        fill_n(mBuffer.begin() + bufferSize, 64 - bufferSize, static_cast<byte>(0));
        this->transform(mBuffer.cbegin());
        bufferSize = 0;
    }

    fill_n(mBuffer.begin() + bufferSize, 56 - bufferSize, static_cast<byte>(0));
    for (int i = 0; i < 8; ++i)
        mBuffer[56 + i] = static_cast<byte>(mSize << 3 >> (i * 8));

    this->transform(mBuffer.cbegin());
    mValid = true;
}

void MD5::reset()
{
    mA     = 0x67452301;
    mB     = 0xefcdab89;
    mC     = 0x98badcfe;
    mD     = 0x10325476;
    mSize  = 0;
    mValid = false;
}

void MD5::compute(const Data &data)
{
    this->reset();
    this->update(data);
    this->finalize();
}

void MD5::compute(string_view data)
{
    Data bytes;
    bytes.reserve(data.size());
    std::transform(data.begin(), data.end(), back_inserter(bytes), [](char c) { return static_cast<byte>(c); });
    this->compute(bytes);
}

void MD5::compute(istream &file)
{
    this->reset();

    Data buffer(64);
    while (file.good())
    {
        file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        if (file.gcount() < static_cast<ptrdiff_t>(buffer.size()))
            buffer.resize(file.gcount());
        this->update(buffer);
    }

    this->finalize();
}

Data MD5::digest() const
{
    if (!mValid)
        throw ChecksumError("MD5: cannot get digest of non-finalized checksum");

    Data               result(16);
    array<uint32_t, 4> digest = {mA, mB, mC, mD};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result[i * 4 + j] = static_cast<byte>(digest[i] >> (j * 8));

    return result;
}

template <typename Iterator>
    requires random_access_iterator<Iterator>
void MD5::transform(const Iterator &data)
{
    static constexpr auto F    = [](uint32_t x, uint32_t y, uint32_t z) -> uint32_t { return z ^ (x & (y ^ z)); };
    static constexpr auto G    = [](uint32_t x, uint32_t y, uint32_t z) -> uint32_t { return y ^ (z & (x ^ y)); };
    static constexpr auto H    = [](uint32_t x, uint32_t y, uint32_t z) -> uint32_t { return x ^ y ^ z; };
    static constexpr auto I    = [](uint32_t x, uint32_t y, uint32_t z) -> uint32_t { return y ^ (x | ~z); };
    const auto            word = [&](int index) -> uint32_t { return reinterpret_cast<const uint32_t &>(data[index * 4]); };
    static constexpr auto step = [](const auto &f, auto &a, auto &b, auto &c, auto &d, auto x, auto k, auto s) -> void
    {
        a += f(b, c, d) + x + k;
        a  = (a << s) | (a >> (32 - s));
        a += b;
    };

    uint32_t a = mA;
    uint32_t b = mB;
    uint32_t c = mC;
    uint32_t d = mD;

    /* clang-format off */
    step(F, a, b, c, d, word(0),  0xd76aa478,  7);
    step(F, d, a, b, c, word(1),  0xe8c7b756, 12);
    step(F, c, d, a, b, word(2),  0x242070db, 17);
    step(F, b, c, d, a, word(3),  0xc1bdceee, 22);
    step(F, a, b, c, d, word(4),  0xf57c0faf,  7);
    step(F, d, a, b, c, word(5),  0x4787c62a, 12);
    step(F, c, d, a, b, word(6),  0xa8304613, 17);
    step(F, b, c, d, a, word(7),  0xfd469501, 22);
    step(F, a, b, c, d, word(8),  0x698098d8,  7);
    step(F, d, a, b, c, word(9),  0x8b44f7af, 12);
    step(F, c, d, a, b, word(10), 0xffff5bb1, 17);
    step(F, b, c, d, a, word(11), 0x895cd7be, 22);
    step(F, a, b, c, d, word(12), 0x6b901122,  7);
    step(F, d, a, b, c, word(13), 0xfd987193, 12);
    step(F, c, d, a, b, word(14), 0xa679438e, 17);
    step(F, b, c, d, a, word(15), 0x49b40821, 22);
    step(G, a, b, c, d, word(1),  0xf61e2562,  5);
    step(G, d, a, b, c, word(6),  0xc040b340,  9);
    step(G, c, d, a, b, word(11), 0x265e5a51, 14);
    step(G, b, c, d, a, word(0),  0xe9b6c7aa, 20);
    step(G, a, b, c, d, word(5),  0xd62f105d,  5);
    step(G, d, a, b, c, word(10), 0x02441453,  9);
    step(G, c, d, a, b, word(15), 0xd8a1e681, 14);
    step(G, b, c, d, a, word(4),  0xe7d3fbc8, 20);
    step(G, a, b, c, d, word(9),  0x21e1cde6,  5);
    step(G, d, a, b, c, word(14), 0xc33707d6,  9);
    step(G, c, d, a, b, word(3),  0xf4d50d87, 14);
    step(G, b, c, d, a, word(8),  0x455a14ed, 20);
    step(G, a, b, c, d, word(13), 0xa9e3e905,  5);
    step(G, d, a, b, c, word(2),  0xfcefa3f8,  9);
    step(G, c, d, a, b, word(7),  0x676f02d9, 14);
    step(G, b, c, d, a, word(12), 0x8d2a4c8a, 20);
    step(H, a, b, c, d, word(5),  0xfffa3942,  4);
    step(H, d, a, b, c, word(8),  0x8771f681, 11);
    step(H, c, d, a, b, word(11), 0x6d9d6122, 16);
    step(H, b, c, d, a, word(14), 0xfde5380c, 23);
    step(H, a, b, c, d, word(1),  0xa4beea44,  4);
    step(H, d, a, b, c, word(4),  0x4bdecfa9, 11);
    step(H, c, d, a, b, word(7),  0xf6bb4b60, 16);
    step(H, b, c, d, a, word(10), 0xbebfbc70, 23);
    step(H, a, b, c, d, word(13), 0x289b7ec6,  4);
    step(H, d, a, b, c, word(0),  0xeaa127fa, 11);
    step(H, c, d, a, b, word(3),  0xd4ef3085, 16);
    step(H, b, c, d, a, word(6),  0x04881d05, 23);
    step(H, a, b, c, d, word(9),  0xd9d4d039,  4);
    step(H, d, a, b, c, word(12), 0xe6db99e5, 11);
    step(H, c, d, a, b, word(15), 0x1fa27cf8, 16);
    step(H, b, c, d, a, word(2),  0xc4ac5665, 23);
    step(I, a, b, c, d, word(0),  0xf4292244,  6);
    step(I, d, a, b, c, word(7),  0x432aff97, 10);
    step(I, c, d, a, b, word(14), 0xab9423a7, 15);
    step(I, b, c, d, a, word(5),  0xfc93a039, 21);
    step(I, a, b, c, d, word(12), 0x655b59c3,  6);
    step(I, d, a, b, c, word(3),  0x8f0ccc92, 10);
    step(I, c, d, a, b, word(10), 0xffeff47d, 15);
    step(I, b, c, d, a, word(1),  0x85845dd1, 21);
    step(I, a, b, c, d, word(8),  0x6fa87e4f,  6);
    step(I, d, a, b, c, word(15), 0xfe2ce6e0, 10);
    step(I, c, d, a, b, word(6),  0xa3014314, 15);
    step(I, b, c, d, a, word(13), 0x4e0811a1, 21);
    step(I, a, b, c, d, word(4),  0xf7537e82,  6);
    step(I, d, a, b, c, word(11), 0xbd3af235, 10);
    step(I, c, d, a, b, word(2),  0x2ad7d2bb, 15);
    step(I, b, c, d, a, word(9),  0xeb86d391, 21);
    /* clang-format on */

    mA += a;
    mB += b;
    mC += c;
    mD += d;
}

CRC32::CRC32() : mValid(false) {}

void CRC32::compute(const Data &data) { this->compute(data.cbegin(), data.cend()); }

void CRC32::compute(const Data::const_iterator &begin, const Data::const_iterator &end)
{
    /* clang-format off */
    constexpr static array<uint32_t, 256> table = {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005, 
        0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 
        0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75, 
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd, 
        0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 
        0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d, 
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95, 
        0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 
        0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072, 
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 
        0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 
        0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba, 
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692, 
        0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 
        0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a, 
        0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 
        0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53, 
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b, 
        0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 
        0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b, 
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3, 
        0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 
        0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3, 
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 
        0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 
        0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec, 
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 
        0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 
        0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c, 
        0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
    };
    /* clang-format on */

    mCRC = 0xffffffff;
    for (auto it = begin; it != end; ++it)
        mCRC = (mCRC << 8) ^ table[(mCRC >> 24) ^ static_cast<uint8_t>(*it)];
    mCRC ^= 0xffffffff;

    mValid = true;
}

void CRC32::compute(std::string_view data)
{
    Data bytes;
    bytes.reserve(data.size());
    std::transform(data.begin(), data.end(), back_inserter(bytes), [](char c) { return static_cast<byte>(c); });
    this->compute(bytes);
}

Data CRC32::digest() const
{
    if (!mValid)
        throw ChecksumError("CRC32: cannot get digest of uninitialized checksum");

    Data result(4);
    for (int i = 0; i < 4; ++i)
        result[3 - i] = static_cast<byte>(mCRC >> (i * 8));
    return result;
}
