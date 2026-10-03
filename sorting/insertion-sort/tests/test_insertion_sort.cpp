// test_insertion_sort.cpp - both sorts against std::stable_sort on 5,000
// random inputs of (key, position) pairs, in std::vector and std::list.
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <list>
#include <utility>
#include <vector>
#include "insertion_sort.hpp"

int main()
{
    std::uint64_t xs = 0x9E3779B97F4A7C15ULL;
    auto next = [&xs] { xs ^= xs << 13; xs ^= xs >> 7; xs ^= xs << 17; return xs; };
    auto by_key = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.first < b.first;
    };
    for (int t = 0; t < 5000; ++t) {
        std::vector<std::pair<int, int>> v(next() % 100);
        for (std::size_t i = 0; i < v.size(); ++i)
            v[i] = {static_cast<int>(next() % 6), static_cast<int>(i)};
        auto expected = v, b = v;
        std::stable_sort(expected.begin(), expected.end(), by_key);
        insertion_sort(v.begin(), v.end(), by_key);
        binary_insertion_sort(b.begin(), b.end(), by_key);
        std::list<std::pair<int, int>> l(expected.begin(), expected.end());
        l.reverse();
        insertion_sort(l.begin(), l.end(), by_key);
        std::vector<std::pair<int, int>> lv(l.begin(), l.end());
        auto expected_l = std::vector<std::pair<int, int>>(expected.rbegin(), expected.rend());
        std::stable_sort(expected_l.begin(), expected_l.end(), by_key);
        if (v != expected || b != expected || lv != expected_l) {
            std::cout << "FAIL on case " << t << '\n';
            return 1;
        }
    }
    std::cout << "passed: 5000 inputs, vector and list, stability checked\n";
    return 0;
}
