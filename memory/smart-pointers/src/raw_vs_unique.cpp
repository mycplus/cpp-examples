// raw_vs_unique.cpp - the same function written with new/delete and with
// std::unique_ptr. Only the early-return path differs.
#include <cstdio>
#include <memory>
#include <string>
#include <utility>

struct Buffer {
    explicit Buffer(std::string n) : name(std::move(n)) { std::printf("  acquire %s\n", name.c_str()); }
    ~Buffer() { std::printf("  release %s\n", name.c_str()); }
    std::string name;
};

bool parse_raw(bool bad_input)
{
    Buffer* buf = new Buffer("raw");
    if (bad_input)
        return false;            // returns without delete: the Buffer leaks
    delete buf;
    return true;
}

bool parse_unique(bool bad_input)
{
    auto buf = std::make_unique<Buffer>("unique");
    if (bad_input)
        return false;            // ~unique_ptr runs here
    return true;                 // and here
}

int main()
{
    std::puts("parse_raw(bad input):");
    parse_raw(true);
    std::puts("parse_unique(bad input):");
    parse_unique(true);
}
