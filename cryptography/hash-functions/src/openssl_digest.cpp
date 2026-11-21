// openssl_digest.cpp - the same digests from OpenSSL's EVP interface, and a check that they
// match the code in digest.hpp. This is what production code should call.
// Build: g++ -std=c++17 -Iinclude src/openssl_digest.cpp -o openssl_digest -lcrypto
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include "digest.hpp"

namespace {

struct CtxDeleter {
    void operator()(EVP_MD_CTX* c) const { EVP_MD_CTX_free(c); }
};
using Ctx = std::unique_ptr<EVP_MD_CTX, CtxDeleter>;

// Streaming digest through EVP. Throws if OpenSSL reports an error.
std::string openssl_hex(const EVP_MD* type, std::string_view data)
{
    Ctx ctx(EVP_MD_CTX_new());
    if (!ctx || EVP_DigestInit_ex(ctx.get(), type, nullptr) != 1 ||
        EVP_DigestUpdate(ctx.get(), data.data(), data.size()) != 1)
        throw std::runtime_error("EVP digest failed");
    std::array<unsigned char, EVP_MAX_MD_SIZE> out{};
    unsigned int n = 0;
    if (EVP_DigestFinal_ex(ctx.get(), out.data(), &n) != 1)
        throw std::runtime_error("EVP digest failed");
    static constexpr char hex[] = "0123456789abcdef";
    std::string s;
    for (unsigned int i = 0; i < n; ++i) {
        s += hex[out[i] >> 4];
        s += hex[out[i] & 15];
    }
    return s;
}

std::string openssl_hmac_sha256(std::string_view key, std::string_view data)
{
    std::array<unsigned char, EVP_MAX_MD_SIZE> out{};
    unsigned int n = 0;
    if (HMAC(EVP_sha256(), key.data(), static_cast<int>(key.size()),
             reinterpret_cast<const unsigned char*>(data.data()), data.size(), out.data(), &n) == nullptr)
        throw std::runtime_error("HMAC failed");
    std::array<std::uint8_t, 32> a{};
    std::copy(out.begin(), out.begin() + 32, a.begin());
    return mycplus::to_hex(a);
}

void compare(const char* label, const std::string& openssl, const std::string& ours)
{
    std::printf("%-12s %s  %s\n", label, openssl.c_str(), openssl == ours ? "matches digest.hpp" : "MISMATCH");
}

}  // namespace

int main()
{
    const std::string_view msg = "abc";
    compare("MD5", openssl_hex(EVP_md5(), msg), mycplus::to_hex(mycplus::hash<mycplus::Md5>(msg)));
    compare("SHA-1", openssl_hex(EVP_sha1(), msg), mycplus::to_hex(mycplus::hash<mycplus::Sha1>(msg)));
    compare("SHA-256", openssl_hex(EVP_sha256(), msg), mycplus::to_hex(mycplus::hash<mycplus::Sha256>(msg)));
    compare("HMAC-SHA-256", openssl_hmac_sha256("Jefe", "what do ya want for nothing?"),
            mycplus::to_hex(mycplus::hmac<mycplus::Sha256>("Jefe", "what do ya want for nothing?")));
}
