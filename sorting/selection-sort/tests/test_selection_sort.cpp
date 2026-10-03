// test_selection_sort.cpp - against std::sort on 5,000 random inputs in
// std::vector and std::forward_list, comparing keys only (not stable).
#include <algorithm>
#include <cstdint>
#include <forward_list>
#include <iostream>
#include <vector>
#include "selection_sort.hpp"

int main()
{
    std::uint64_t xs = 0x9E3779B97F4A7C15ULL;
    auto next = [&xs] { xs ^= xs << 13; xs ^= xs >> 7; xs ^= xs << 17; return xs; };
    for (int t = 0; t < 5000; ++t) {
        std::vector<int> v(next() % 100);
        for (int& x : v) x = static_cast<int>(next() % 50) - 25;
        auto expected = v;
        std::sort(expected.begin(), expected.end());
        std::forward_list<int> fl(v.begin(), v.end());
        selection_sort(v.begin(), v.end());
        selection_sort(fl.begin(), fl.end());
        if (v != expected || !std::equal(fl.begin(), fl.end(), expected.begin(), expected.end())) {
            std::cout << "FAIL on case " << t << '\n';
            return 1;
        }
    }
    std::cout << "passed: 5000 inputs, vector and forward_list\n";
    return 0;
}
