// returning.cpp - returning several values, or a value that may be missing.
#include <iostream>
#include <optional>
#include <string_view>

struct MinMax {
    int min;
    int max;
};

MinMax min_max(std::initializer_list<int> values) {
    MinMax r{*values.begin(), *values.begin()};
    for (int v : values) {
        if (v < r.min) r.min = v;
        if (v > r.max) r.max = v;
    }
    return r;
}

// No sensible int to return for bad input, so say so in the return type.
std::optional<int> parse_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    return std::nullopt;
}

int main() {
    auto [low, high] = min_max({4, -2, 9, 0});   // structured binding (C++17)
    std::cout << "min " << low << ", max " << high << '\n';

    for (char c : std::string_view("7x")) {
        if (auto d = parse_digit(c))
            std::cout << c << " -> " << *d << '\n';
        else
            std::cout << c << " -> not a digit\n";
    }
}
