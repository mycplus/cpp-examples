// test_vectors.cpp - MD5, SHA-1, SHA-256 and HMAC against published and independently
// generated answers, one-shot, in chunks and split at every offset.
// Usage: test_vectors <vectors-directory>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "digest.hpp"

using namespace mycplus;

namespace {

int failures = 0;

void check(bool ok, const std::string& what, const std::string& detail)
{
    if (!ok) {
        ++failures;
        std::printf("FAIL %s: %s\n", what.c_str(), detail.c_str());
    }
}

std::vector<std::string> split_tabs(const std::string& line)
{
    std::vector<std::string> fields;
    std::string field;
    std::istringstream in(line);
    while (std::getline(in, field, '\t'))
        fields.push_back(field);
    if (!line.empty() && line.back() == '\t')
        fields.emplace_back();
    return fields;
}

std::ifstream open_vectors(const std::string& dir, const char* file)
{
    std::ifstream in(dir + "/" + file);
    if (!in) {
        std::fprintf(stderr, "cannot open %s/%s\n", dir.c_str(), file);
        std::exit(2);
    }
    return in;
}

std::string strip_cr(std::string s)
{
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n'))
        s.pop_back();
    return s;
}

std::string unhex(const std::string& hex)
{
    std::string out;
    for (std::size_t i = 0; i + 1 < hex.size(); i += 2)
        out += static_cast<char>(std::stoi(hex.substr(i, 2), nullptr, 16));
    return out;
}

// Runs fn once for the named hash, so the tests are written once for all three.
template <class Fn>
bool with_hash(const std::string& name, Fn&& fn)
{
    if (name == "md5") { fn(Md5{}); return true; }
    if (name == "sha1") { fn(Sha1{}); return true; }
    if (name == "sha256") { fn(Sha256{}); return true; }
    return false;
}

template <class Hash>
std::string hex_of(const std::string& data)
{
    return to_hex(hash<Hash>(data));
}

std::string hex_by_name(const std::string& name, const std::string& data)
{
    std::string out;
    with_hash(name, [&](auto h) { out = hex_of<decltype(h)>(data); });
    return out;
}

// The same message fed in `chunk`-byte pieces.
template <class Hash>
std::string hex_chunked(const std::string& data, std::size_t chunk)
{
    Hash h;
    for (std::size_t i = 0; i < data.size(); i += chunk)
        h.update(data.substr(i, chunk));
    return to_hex(h.finalize());
}

int known_answers(const std::string& dir)
{
    auto in = open_vectors(dir, "known_answers.txt");
    std::string line;
    int count = 0;
    while (std::getline(in, line)) {
        line = strip_cr(line);
        if (line.empty() || line[0] == '#')
            continue;
        const auto f = split_tabs(line);
        if (f.size() != 4)
            continue;
        std::string message;
        for (unsigned long i = 0, n = std::stoul(f[1]); i < n; ++i)
            message += f[2];
        check(hex_by_name(f[0], message) == f[3], f[0], f[2]);
        with_hash(f[0], [&](auto h) { check(hex_chunked<decltype(h)>(message, 7) == f[3], f[0], "7-byte chunks"); });
        ++count;
    }
    return count;
}

std::string pattern(std::size_t n)
{
    std::string m(n, '\0');
    for (std::size_t i = 0; i < n; ++i)
        m[i] = static_cast<char>((i * 31 + 7) & 0xFF);
    return m;
}

int lengths(const std::string& dir)
{
    auto in = open_vectors(dir, "lengths.txt");
    std::string line;
    int count = 0;
    const char* names[3] = {"md5", "sha1", "sha256"};
    while (std::getline(in, line)) {
        line = strip_cr(line);
        if (line.empty() || line[0] == '#')
            continue;
        const auto f = split_tabs(line);
        const std::size_t n = std::stoul(f[0]);
        const std::string msg = pattern(n);
        for (int i = 0; i < 3; ++i) {
            check(hex_by_name(names[i], msg) == f[1 + i], names[i], f[0]);
            if (n <= 130) {                       // split the message at every offset
                for (std::size_t cut = 0; cut <= n; ++cut) {
                    with_hash(names[i], [&](auto h) {
                        h.update(msg.substr(0, cut));
                        h.update(msg.substr(cut));
                        check(to_hex(h.finalize()) == f[1 + i], names[i], "split");
                    });
                }
            }
        }
        ++count;
    }
    return count;
}

int hmac_vectors(const std::string& dir)
{
    auto in = open_vectors(dir, "hmac.txt");
    std::string line;
    int count = 0;
    while (std::getline(in, line)) {
        line = strip_cr(line);
        if (line.empty() || line[0] == '#')
            continue;
        const auto f = split_tabs(line);
        const std::string key = unhex(f[1]), data = unhex(f[2]);
        std::string got;
        with_hash(f[0], [&](auto h) { got = to_hex(hmac<decltype(h)>(key, data)); });
        check(got == f[3], "hmac-" + f[0], f[1]);
        ++count;
    }
    return count;
}

}  // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::fprintf(stderr, "usage: test_vectors <vectors-directory>\n");
        return 2;
    }
    const int ka = known_answers(argv[1]);
    const int ln = lengths(argv[1]);
    const int hm = hmac_vectors(argv[1]);

    check(digest_equal(hash<Sha256>("a"), hash<Sha256>("a")) && !digest_equal(hash<Sha256>("a"), hash<Sha256>("b")),
          "digest_equal", "");

    std::printf("known answers: %d\n", ka);
    std::printf("message lengths: %d (each also split at every offset up to 130 bytes)\n", ln);
    std::printf("HMAC vectors: %d\n", hm);
    std::printf("failures: %d\n", failures);
    return failures == 0 ? 0 : 1;
}
