// serpent_speed.cpp - encryption speed in ECB mode over a buffer of N MiB
// (default 16), median of 5 runs. Every language's benchmark fills the buffer
// the same way and prints the XOR of all ciphertext blocks, so the results
// can be compared. Usage: serpent_speed [MiB]
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "serpent.hpp"

int main(int argc, char** argv) {
    using mycplus::Serpent;
    const long mib = argc > 1 ? std::strtol(argv[1], nullptr, 10) : 16;
    if (mib < 1 || mib > 1024)
        return 1;
    const std::size_t bytes = static_cast<std::size_t>(mib) << 20;
    std::vector<Serpent::Block> in(bytes / 16), out(bytes / 16);
    for (std::size_t i = 0; i < bytes; ++i)
        in[i / 16][i % 16] = static_cast<std::uint8_t>(static_cast<std::uint32_t>(i * 2654435761u) >> 24);
    std::array<std::uint8_t, 32> key{};
    for (std::size_t i = 0; i < key.size(); ++i)
        key[i] = static_cast<std::uint8_t>(i);
    const Serpent cipher(key);

    std::vector<double> mbps;
    for (int run = 0; run < 5; ++run) {
        const auto start = std::chrono::steady_clock::now();
        for (std::size_t b = 0; b < in.size(); ++b)
            out[b] = cipher.encrypt(in[b]);
        const std::chrono::duration<double> s = std::chrono::steady_clock::now() - start;
        mbps.push_back(static_cast<double>(bytes) / 1e6 / s.count());
    }
    std::sort(mbps.begin(), mbps.end());
    Serpent::Block fold{};
    for (const auto& block : out)
        for (std::size_t i = 0; i < 16; ++i)
            fold[i] ^= block[i];
    std::printf("C++:         %8.2f MB/s (median of 5, %ld MiB)  xor of blocks ", mbps[2], mib);
    for (std::uint8_t b : fold)
        std::printf("%02x", b);
    std::printf("\n");
}
