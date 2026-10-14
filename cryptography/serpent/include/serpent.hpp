// serpent.hpp - the Serpent block cipher (128-bit blocks, keys up to 256 bits)
// in C++17. Bytes are read as little-endian 32-bit words, the order the
// NESSIE test vectors and libgcrypt use.
//
// For learning and testing. It has not been reviewed for side channels; use a
// maintained cryptographic library for anything that protects real data.
#ifndef MYCPLUS_SERPENT_HPP
#define MYCPLUS_SERPENT_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>

namespace mycplus {

class Serpent {
public:
    static constexpr std::size_t block_size = 16;
    static constexpr std::size_t max_key_size = 32;
    using Block = std::array<std::uint8_t, block_size>;

    // Expands a key of 1 to 32 bytes; throws std::invalid_argument otherwise.
    Serpent(const std::uint8_t* key, std::size_t key_len);

    // Any contiguous byte container: std::array, std::vector, std::string_view...
    template <class Bytes>
    explicit Serpent(const Bytes& key)
        : Serpent(reinterpret_cast<const std::uint8_t*>(std::data(key)), std::size(key)) {}

    ~Serpent();                                  // wipes the subkeys
    Serpent(const Serpent&) = default;
    Serpent& operator=(const Serpent&) = default;

    [[nodiscard]] Block encrypt(const Block& in) const;
    [[nodiscard]] Block decrypt(const Block& in) const;

private:
    using Words = std::array<std::uint32_t, 4>;
    std::array<Words, 33> subkeys_{};            // K0..K32
};

namespace detail {

using Words = std::array<std::uint32_t, 4>;

inline constexpr std::uint8_t sbox[8][16] = {
    { 3, 8, 15, 1, 10, 6, 5, 11, 14, 13, 4, 2, 7, 0, 9, 12 },
    { 15, 12, 2, 7, 9, 0, 5, 10, 1, 11, 14, 8, 6, 13, 3, 4 },
    { 8, 6, 7, 9, 3, 12, 10, 15, 13, 1, 14, 4, 0, 11, 5, 2 },
    { 0, 15, 11, 8, 12, 9, 6, 3, 13, 1, 2, 4, 10, 7, 5, 14 },
    { 1, 15, 8, 3, 12, 0, 11, 6, 2, 5, 4, 10, 9, 14, 7, 13 },
    { 15, 5, 2, 11, 4, 10, 9, 12, 0, 3, 14, 8, 13, 6, 7, 1 },
    { 7, 2, 12, 5, 8, 4, 6, 11, 14, 9, 1, 15, 13, 3, 10, 0 },
    { 1, 13, 15, 0, 14, 8, 2, 11, 7, 4, 12, 10, 9, 3, 5, 6 },
};

// The inverse S-boxes are computed from the forward ones at compile time,
// so there is no second table to mistype.
struct InverseSboxes {
    std::uint8_t box[8][16]{};
    constexpr InverseSboxes() {
        for (int s = 0; s < 8; ++s)
            for (int i = 0; i < 16; ++i)
                box[s][sbox[s][i]] = static_cast<std::uint8_t>(i);
    }
};
inline constexpr InverseSboxes sbox_inv{};

constexpr std::uint32_t phi = 0x9e3779b9u;       // fractional part of the golden ratio

constexpr std::uint32_t rotl(std::uint32_t x, unsigned n) { return (x << n) | (x >> (32 - n)); }
constexpr std::uint32_t rotr(std::uint32_t x, unsigned n) { return (x >> n) | (x << (32 - n)); }

inline std::uint32_t load_le32(const std::uint8_t* p) {
    return std::uint32_t{p[0]} | std::uint32_t{p[1]} << 8 | std::uint32_t{p[2]} << 16 |
           std::uint32_t{p[3]} << 24;
}

inline void store_le32(std::uint8_t* p, std::uint32_t v) {
    for (int i = 0; i < 4; ++i)
        p[i] = static_cast<std::uint8_t>(v >> (8 * i));
}

// Bit j of x[0..3] forms a 4-bit input (x[0] is the low bit); all 32 bit
// positions go through the same table.
inline void apply_sbox(const std::uint8_t (&box)[16], Words& x) {
    Words y{};
    for (unsigned j = 0; j < 32; ++j) {
        unsigned in = 0;
        for (unsigned b = 0; b < 4; ++b)
            in |= ((x[b] >> j) & 1u) << b;
        const unsigned out = box[in];
        for (unsigned b = 0; b < 4; ++b)
            y[b] |= std::uint32_t{(out >> b) & 1u} << j;
    }
    x = y;
}

inline void linear_transform(Words& x) {
    x[0] = rotl(x[0], 13);
    x[2] = rotl(x[2], 3);
    x[1] ^= x[0] ^ x[2];
    x[3] ^= x[2] ^ (x[0] << 3);
    x[1] = rotl(x[1], 1);
    x[3] = rotl(x[3], 7);
    x[0] ^= x[1] ^ x[3];
    x[2] ^= x[3] ^ (x[1] << 7);
    x[0] = rotl(x[0], 5);
    x[2] = rotl(x[2], 22);
}

inline void inverse_linear_transform(Words& x) {
    x[2] = rotr(x[2], 22);
    x[0] = rotr(x[0], 5);
    x[2] ^= x[3] ^ (x[1] << 7);
    x[0] ^= x[1] ^ x[3];
    x[3] = rotr(x[3], 7);
    x[1] = rotr(x[1], 1);
    x[3] ^= x[2] ^ (x[0] << 3);
    x[1] ^= x[0] ^ x[2];
    x[2] = rotr(x[2], 3);
    x[0] = rotr(x[0], 13);
}

inline void xor_key(Words& x, const Words& k) {
    for (std::size_t i = 0; i < 4; ++i)
        x[i] ^= k[i];
}

}  // namespace detail

inline Serpent::Serpent(const std::uint8_t* key, std::size_t key_len) {
    using namespace detail;
    if (key == nullptr || key_len == 0 || key_len > max_key_size)
        throw std::invalid_argument("Serpent keys are 1 to 32 bytes");

    // Pad to 256 bits: one 1 bit after the key, then zeros.
    std::array<std::uint8_t, max_key_size> padded{};
    for (std::size_t i = 0; i < key_len; ++i)
        padded[i] = key[i];
    if (key_len < max_key_size)
        padded[key_len] = 0x01;

    // w[0..7] hold the padded key; w[8..139] are the 132 prekey words.
    std::array<std::uint32_t, 140> w{};
    for (std::size_t i = 0; i < 8; ++i)
        w[i] = load_le32(&padded[4 * i]);
    for (std::size_t i = 8; i < w.size(); ++i)
        w[i] = rotl(w[i - 8] ^ w[i - 5] ^ w[i - 3] ^ w[i - 1] ^ phi ^
                        static_cast<std::uint32_t>(i - 8), 11);

    // Subkey i passes prekeys 4i..4i+3 through S-box (3 - i) mod 8.
    for (std::size_t i = 0; i < subkeys_.size(); ++i) {
        subkeys_[i] = {w[8 + 4 * i], w[9 + 4 * i], w[10 + 4 * i], w[11 + 4 * i]};
        apply_sbox(sbox[(35 - i) % 8], subkeys_[i]);   // 35 = 3 mod 8, never negative
    }
}

inline Serpent::~Serpent() {
    // volatile stops the compiler removing stores to memory about to be freed
    volatile std::uint32_t* p = &subkeys_[0][0];
    for (std::size_t i = 0; i < 33 * 4; ++i)
        p[i] = 0;
}

inline Serpent::Block Serpent::encrypt(const Block& in) const {
    using namespace detail;
    Words x{};
    for (std::size_t i = 0; i < 4; ++i)
        x[i] = load_le32(&in[4 * i]);
    for (std::size_t r = 0; r < 32; ++r) {
        xor_key(x, subkeys_[r]);
        apply_sbox(sbox[r % 8], x);
        if (r < 31)
            linear_transform(x);
        else
            xor_key(x, subkeys_[32]);            // the last round replaces LT with K32
    }
    Block out{};
    for (std::size_t i = 0; i < 4; ++i)
        store_le32(&out[4 * i], x[i]);
    return out;
}

inline Serpent::Block Serpent::decrypt(const Block& in) const {
    using namespace detail;
    Words x{};
    for (std::size_t i = 0; i < 4; ++i)
        x[i] = load_le32(&in[4 * i]);
    for (std::size_t r = 32; r-- > 0;) {
        if (r == 31)
            xor_key(x, subkeys_[32]);
        else
            inverse_linear_transform(x);
        apply_sbox(sbox_inv.box[r % 8], x);
        xor_key(x, subkeys_[r]);
    }
    Block out{};
    for (std::size_t i = 0; i < 4; ++i)
        store_le32(&out[4 * i], x[i]);
    return out;
}

}  // namespace mycplus

#endif
