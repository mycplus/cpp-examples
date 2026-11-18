// unions_variant.cpp - a union stores one of several members in the same
// bytes and does not record which one. A tagged union adds that record by
// hand; std::variant does it automatically and checks every access.
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

union Number {
    std::int32_t i;
    float f;
};

// A tagged union: the enum says which member of the union is active.
struct Setting {
    enum class Kind { integer, real } kind;
    union {
        int i;
        double d;
    } value;
};

void print(const Setting& s)
{
    switch (s.kind) {
    case Setting::Kind::integer: std::cout << "integer " << s.value.i << '\n'; break;
    case Setting::Kind::real:    std::cout << "real " << s.value.d << '\n'; break;
    }
}

// The same idea with std::variant: the type remembers which alternative it holds.
using Value = std::variant<int, double, std::string>;

struct Describe {
    void operator()(int v) const { std::cout << "int " << v << '\n'; }
    void operator()(double v) const { std::cout << "double " << v << '\n'; }
    void operator()(const std::string& v) const { std::cout << "string \"" << v << "\"\n"; }
};

int main()
{
    std::cout << "sizeof(Number) = " << sizeof(Number) << " (members share the same 4 bytes)\n";

    Number n;
    n.f = 1.0f;     // f is now the active member; reading n.i would be undefined behavior

    // To see a float's bits, copy them into an integer instead of reading the
    // inactive member. (C++20 adds std::bit_cast for the same job.)
    std::uint32_t bits = 0;
    static_assert(sizeof bits == sizeof n.f);
    std::memcpy(&bits, &n.f, sizeof bits);
    std::cout << "bits of 1.0f = 0x" << std::hex << std::setw(8) << std::setfill('0') << bits
              << std::dec << std::setfill(' ') << '\n';

    Setting timeout{Setting::Kind::integer, {}};
    timeout.value.i = 30;
    Setting ratio{Setting::Kind::real, {}};
    ratio.value.d = 0.75;
    print(timeout);
    print(ratio);

    std::vector<Value> values{42, 2.5, std::string("blue")};
    for (const Value& v : values)
        std::visit(Describe{}, v);

    Value v = 2.5;
    std::cout << "holds double? " << std::boolalpha << std::holds_alternative<double>(v) << '\n';
    try {
        std::cout << std::get<int>(v) << '\n';      // wrong alternative
    } catch (const std::bad_variant_access&) {
        std::cout << "std::get<int> on a double: std::bad_variant_access\n";
    }
}
