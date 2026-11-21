// digest.hpp - MD5, SHA-1 and SHA-256 (C++17, header only), plus HMAC.
// For learning and testing. To protect real data, use a maintained library.
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>

namespace mycplus {

namespace detail {

constexpr std::uint32_t rotl(std::uint32_t x, unsigned n) { return (x << n) | (x >> (32 - n)); }
constexpr std::uint32_t rotr(std::uint32_t x, unsigned n) { return (x >> n) | (x << (32 - n)); }

constexpr std::uint32_t load_le(const std::uint8_t* p)
{
    return std::uint32_t{p[0]} | std::uint32_t{p[1]} << 8 | std::uint32_t{p[2]} << 16 | std::uint32_t{p[3]} << 24;
}
constexpr std::uint32_t load_be(const std::uint8_t* p)
{
    return std::uint32_t{p[3]} | std::uint32_t{p[2]} << 8 | std::uint32_t{p[1]} << 16 | std::uint32_t{p[0]} << 24;
}

}  // namespace detail

// What MD5, SHA-1 and SHA-256 share: 64-byte blocks, a padded final block, a 64-bit length.
// Derived supplies the initial state and static compress(state, block).
template <class Derived, std::size_t DigestBytes, bool BigEndian>
class BlockHash {
public:
    static constexpr std::size_t digest_size = DigestBytes;
    static constexpr std::size_t block_size = 64;
    using Digest = std::array<std::uint8_t, DigestBytes>;
    using State = std::array<std::uint32_t, DigestBytes / 4>;

    void update(const void* data, std::size_t len)
    {
        const auto* p = static_cast<const std::uint8_t*>(data);
        std::size_t used = static_cast<std::size_t>(bytes_ & 63);
        bytes_ += len;
        if (used != 0) {                         // top up a partly filled block first
            const std::size_t take = std::min(block_size - used, len);
            std::memcpy(buffer_.data() + used, p, take);
            p += take;
            len -= take;
            if (used + take < block_size)
                return;
            Derived::compress(state_, buffer_.data());
        }
        for (; len >= block_size; p += block_size, len -= block_size)
            Derived::compress(state_, p);        // whole blocks straight from the input
        std::memcpy(buffer_.data(), p, len);     // keep the tail for next time
    }

    void update(std::string_view text) { update(text.data(), text.size()); }

    // Pads, finishes and returns the digest. The object must not be reused afterwards.
    [[nodiscard]] Digest finalize()
    {
        const std::uint64_t bits = bytes_ * 8;
        std::array<std::uint8_t, 72> pad{};
        pad[0] = 0x80;
        const std::size_t zeros = static_cast<std::size_t>((55 - (bytes_ & 63)) & 63);
        const std::size_t n = 1 + zeros;
        for (std::size_t i = 0; i < 8; ++i)
            pad[n + i] = static_cast<std::uint8_t>(bits >> (BigEndian ? 56 - 8 * i : 8 * i));
        update(pad.data(), n + 8);               // the padding completes the last block

        Digest out{};
        for (std::size_t i = 0; i < state_.size(); ++i)
            for (std::size_t b = 0; b < 4; ++b)
                out[4 * i + b] = static_cast<std::uint8_t>(state_[i] >> (BigEndian ? 24 - 8 * b : 8 * b));
        return out;
    }

protected:
    State state_{};

private:
    std::uint64_t bytes_ = 0;
    std::array<std::uint8_t, block_size> buffer_{};
};

class Md5 : public BlockHash<Md5, 16, false> {
public:
    Md5() { state_ = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476}; }

    static void compress(State& h, const std::uint8_t* block)
    {
        static constexpr std::array<std::uint32_t, 64> k = {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
            0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
            0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
            0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
            0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
            0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
            0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
            0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
            0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
            0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
            0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
            0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
            0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
            0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
            0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
            0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
        };
        static constexpr std::array<std::uint8_t, 64> shift = {
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
            5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
        std::array<std::uint32_t, 16> m{};
        for (std::size_t i = 0; i < 16; ++i)
            m[i] = detail::load_le(block + 4 * i);

        std::uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        for (unsigned i = 0; i < 64; ++i) {
            std::uint32_t f = 0;
            unsigned g = 0;
            if (i < 16)      { f = (b & c) | (~b & d); g = i; }
            else if (i < 32) { f = (d & b) | (~d & c); g = (5 * i + 1) & 15; }
            else if (i < 48) { f = b ^ c ^ d;          g = (3 * i + 5) & 15; }
            else             { f = c ^ (b | ~d);       g = (7 * i) & 15; }
            f += a + k[i] + m[g];
            a = d;
            d = c;
            c = b;
            b += detail::rotl(f, shift[i]);
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
    }
};

class Sha1 : public BlockHash<Sha1, 20, true> {
public:
    Sha1() { state_ = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0}; }

    static void compress(State& h, const std::uint8_t* block)
    {
        std::array<std::uint32_t, 80> w{};
        for (std::size_t t = 0; t < 16; ++t)
            w[t] = detail::load_be(block + 4 * t);
        for (std::size_t t = 16; t < 80; ++t)
            w[t] = detail::rotl(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);

        std::uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
        for (std::size_t t = 0; t < 80; ++t) {
            std::uint32_t f = 0, k = 0;
            if (t < 20)      { f = (b & c) | (~b & d);          k = 0x5a827999; }
            else if (t < 40) { f = b ^ c ^ d;                   k = 0x6ed9eba1; }
            else if (t < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdc; }
            else             { f = b ^ c ^ d;                   k = 0xca62c1d6; }
            const std::uint32_t temp = detail::rotl(a, 5) + f + e + k + w[t];
            e = d;
            d = c;
            c = detail::rotl(b, 30);
            b = a;
            a = temp;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
    }
};

class Sha256 : public BlockHash<Sha256, 32, true> {
public:
    Sha256()
    {
        state_ = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
        };
    }

    static void compress(State& h, const std::uint8_t* block)
    {
        static constexpr std::array<std::uint32_t, 64> k = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
            0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
            0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
            0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
            0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
            0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
            0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
            0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
            0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
        };
        std::array<std::uint32_t, 64> w{};
        for (std::size_t t = 0; t < 16; ++t)
            w[t] = detail::load_be(block + 4 * t);
        for (std::size_t t = 16; t < 64; ++t) {
            const std::uint32_t s0 = detail::rotr(w[t - 15], 7) ^ detail::rotr(w[t - 15], 18) ^ (w[t - 15] >> 3);
            const std::uint32_t s1 = detail::rotr(w[t - 2], 17) ^ detail::rotr(w[t - 2], 19) ^ (w[t - 2] >> 10);
            w[t] = w[t - 16] + s0 + w[t - 7] + s1;
        }

        std::uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
        for (std::size_t t = 0; t < 64; ++t) {
            const std::uint32_t big1 = detail::rotr(e, 6) ^ detail::rotr(e, 11) ^ detail::rotr(e, 25);
            const std::uint32_t ch = (e & f) ^ (~e & g);
            const std::uint32_t t1 = hh + big1 + ch + k[t] + w[t];
            const std::uint32_t big0 = detail::rotr(a, 2) ^ detail::rotr(a, 13) ^ detail::rotr(a, 22);
            const std::uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            const std::uint32_t t2 = big0 + maj;
            hh = g; g = f; f = e; e = d + t1;
            d = c; c = b; b = a; a = t1 + t2;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }
};

// One-shot: mycplus::hash<Sha256>("abc").
template <class Hash>
[[nodiscard]] typename Hash::Digest hash(std::string_view data)
{
    Hash h;
    h.update(data);
    return h.finalize();
}

// HMAC (RFC 2104): H((K ^ opad) || H((K ^ ipad) || message)).
template <class Hash>
[[nodiscard]] typename Hash::Digest hmac(std::string_view key, std::string_view message)
{
    std::array<std::uint8_t, Hash::block_size> k{};
    if (key.size() > k.size()) {                       // a key longer than a block is hashed first
        const auto digest = hash<Hash>(key);
        std::copy(digest.begin(), digest.end(), k.begin());
    } else {
        std::copy(key.begin(), key.end(), k.begin());
    }
    std::array<std::uint8_t, Hash::block_size> pad{};
    for (std::size_t i = 0; i < pad.size(); ++i)
        pad[i] = k[i] ^ 0x36;
    Hash inner;
    inner.update(pad.data(), pad.size());
    inner.update(message);
    const auto inner_digest = inner.finalize();

    for (std::size_t i = 0; i < pad.size(); ++i)
        pad[i] = k[i] ^ 0x5c;
    Hash outer;
    outer.update(pad.data(), pad.size());
    outer.update(inner_digest.data(), inner_digest.size());
    return outer.finalize();
}

// Compares two digests in time that depends only on the length.
template <std::size_t N>
[[nodiscard]] bool digest_equal(const std::array<std::uint8_t, N>& a, const std::array<std::uint8_t, N>& b)
{
    std::uint8_t diff = 0;
    for (std::size_t i = 0; i < N; ++i)
        diff = static_cast<std::uint8_t>(diff | (a[i] ^ b[i]));   // never stops early
    return diff == 0;
}

template <std::size_t N>
[[nodiscard]] std::string to_hex(const std::array<std::uint8_t, N>& digest)
{
    static constexpr char hex[] = "0123456789abcdef";
    std::string out;
    out.reserve(2 * N);
    for (const std::uint8_t byte : digest) {
        out += hex[byte >> 4];
        out += hex[byte & 15];
    }
    return out;
}

}  // namespace mycplus
