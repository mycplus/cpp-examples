// digest_demo.cpp - MD5, SHA-1 and SHA-256 of three short messages.
// Build: g++ -std=c++17 -Wall -Wextra -Iinclude src/digest_demo.cpp -o digest_demo
#include <cstdio>
#include <string_view>

#include "digest.hpp"

static void show(std::string_view text)
{
    std::printf("message: \"%.*s\" (%zu bytes)\n", static_cast<int>(text.size()), text.data(), text.size());
    std::printf("  MD5     %s\n", mycplus::to_hex(mycplus::hash<mycplus::Md5>(text)).c_str());
    std::printf("  SHA-1   %s\n", mycplus::to_hex(mycplus::hash<mycplus::Sha1>(text)).c_str());
    std::printf("  SHA-256 %s\n", mycplus::to_hex(mycplus::hash<mycplus::Sha256>(text)).c_str());
}

int main()
{
    show("");
    show("abc");
    show("The quick brown fox jumps over the lazy dog");
}
