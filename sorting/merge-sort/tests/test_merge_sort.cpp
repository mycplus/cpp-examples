// test_merge_sort.cpp - against std::stable_sort on 5,000 random inputs of
// (key, position) pairs, which checks stability as well as order.
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>
#include "merge_sort.hpp"

int main()
{
    std::uint64_t xs = 0x9E3779B97F4A7C15ULL;
    auto next = [&xs] { xs ^= xs << 13; xs ^= xs >> 7; xs ^= xs << 17; return xs; };
    auto by_key = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.first < b.first;
    };
    for (int t = 0; t < 5000; ++t) {
        std::vector<std::pair<int, int>> v(next() % 200);
        for (std::size_t i = 0; i < v.size(); ++i)
            v[i] = {static_cast<int>(next() % 8), static_cast<int>(i)};
        auto expected = v;
        std::stable_sort(expected.begin(), expected.end(), by_key);
        merge_sort(v.begin(), v.end(), by_key);
        if (v != expected) {
            std::cout << "FAIL on case " << t << '\n';
            return 1;
        }
    }
    std::cout << "passed: 5000 inputs, stability checked\n";
    return 0;
}
