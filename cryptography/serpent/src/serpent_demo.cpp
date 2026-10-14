// serpent_demo.cpp - encrypts and decrypts one block with a 256-bit key, then
// checks two published test vectors: one in NESSIE byte order, and one from
// the Serpent AES submission, which writes every value byte-reversed.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic -I../include serpent_demo.cpp -o serpent_demo
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <string_view>

#include "serpent.hpp"

using mycplus::Serpent;

template <class Bytes>
void print_hex(const char* label, const Bytes& bytes) {
    std::printf("%-11s", label);
    for (std::uint8_t b : bytes)
        std::printf("%02x", b);
    std::printf("\n");
}

int main() {
    std::array<std::uint8_t, 32> key{};
    for (std::size_t i = 0; i < key.size(); ++i)
        key[i] = static_cast<std::uint8_t>(i);   // 00 01 02 ... 1f
    constexpr std::string_view text = "Serpent: 128-bit";
    static_assert(text.size() == Serpent::block_size);
    Serpent::Block plain{};
    std::copy(text.begin(), text.end(), plain.begin());

    const Serpent cipher(key);
    const Serpent::Block encrypted = cipher.encrypt(plain);
    const Serpent::Block back = cipher.decrypt(encrypted);

    print_hex("key", key);
    print_hex("plaintext", plain);
    print_hex("ciphertext", encrypted);
    std::printf("decrypts back: %s\n", back == plain ? "yes" : "no");

    // NESSIE: 128-bit key 80 00 .. 00, zero block -> 264e5481...
    const std::array<std::uint8_t, 16> k128{0x80};
    const Serpent::Block nessie{0x26, 0x4e, 0x54, 0x81, 0xef, 0xf4, 0x2a, 0x46,
                                0x06, 0xab, 0xda, 0x06, 0xc0, 0xbf, 0xda, 0x3d};
    const bool nessie_ok = Serpent(k128).encrypt(Serpent::Block{}) == nessie;
    std::printf("NESSIE vector:          %s\n", nessie_ok ? "match" : "MISMATCH");

    // AES submission: 256-bit zero key, zero block -> 89104945... once the
    // output bytes are reversed (the key and block are all zero either way).
    const Serpent::Block submission{0x89, 0x10, 0x49, 0x45, 0x04, 0x18, 0x19, 0x50,
                                    0xf9, 0x8d, 0xd9, 0x98, 0xa8, 0x2b, 0x67, 0x49};
    Serpent::Block out = Serpent(std::array<std::uint8_t, 32>{}).encrypt(Serpent::Block{});
    std::reverse(out.begin(), out.end());
    std::printf("AES submission vector:  %s\n", out == submission ? "match" : "MISMATCH");
}
