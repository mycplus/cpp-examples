// test_vectors.cpp - every NESSIE single-block vector, in both directions,
// plus key-length validation.
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "serpent.hpp"

using mycplus::Serpent;

static std::vector<std::uint8_t> from_hex(const std::string& s) {
    std::vector<std::uint8_t> out;
    for (std::size_t i = 0; i + 1 < s.size(); i += 2)
        out.push_back(static_cast<std::uint8_t>(std::stoul(s.substr(i, 2), nullptr, 16)));
    return out;
}

static Serpent::Block to_block(const std::vector<std::uint8_t>& v) {
    Serpent::Block b{};
    if (v.size() != b.size())
        throw std::runtime_error("bad block in vector file");
    for (std::size_t i = 0; i < b.size(); ++i)
        b[i] = v[i];
    return b;
}

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "tests/vectors/serpent_nessie.txt";
    std::ifstream file(path);
    if (!file) {
        std::perror(path);
        return 1;
    }

    std::size_t total = 0, failures = 0;
    std::map<std::size_t, std::size_t> passed_by_bits;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream fields(line);
        std::string k, p, c;
        if (line.empty() || line[0] == '#' || !(fields >> k >> p >> c))
            continue;
        ++total;
        const auto key = from_hex(k);
        const Serpent::Block plain = to_block(from_hex(p)), expected = to_block(from_hex(c));
        const Serpent cipher(key);
        if (cipher.encrypt(plain) == expected && cipher.decrypt(expected) == plain)
            ++passed_by_bits[key.size() * 8];
        else
            ++failures;
    }
    std::printf("%zu vectors: %zu with 128-bit keys, %zu with 192-bit, %zu with 256-bit; %zu failed\n",
                total, passed_by_bits[128], passed_by_bits[192], passed_by_bits[256], failures);

    auto rejected = [](std::size_t len) {
        const std::vector<std::uint8_t> key(len);
        try {
            const Serpent cipher(key.data(), key.size());
            (void)cipher;
            return false;
        } catch (const std::invalid_argument&) {
            return true;
        }
    };
    const bool bad_lengths = rejected(0) && rejected(33);
    const bool one_byte = !rejected(1);
    std::printf("rejects 0- and 33-byte keys: %s; accepts a 1-byte key: %s\n",
                bad_lengths ? "yes" : "no", one_byte ? "yes" : "no");
    return failures == 0 && total > 0 && bad_lengths && one_byte ? 0 : 1;
}
