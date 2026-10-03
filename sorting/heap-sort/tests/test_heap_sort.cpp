// test_heap_sort.cpp - against std::sort on 5,000 random inputs, with the
// default comparator and std::greater.
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>
#include "heap_sort.hpp"

int main()
{
    std::uint64_t xs = 0x9E3779B97F4A7C15ULL;
    auto next = [&xs] { xs ^= xs << 13; xs ^= xs >> 7; xs ^= xs << 17; return xs; };
    for (int t = 0; t < 5000; ++t) {
        std::vector<int> v(next() % 200);
        for (int& x : v) x = static_cast<int>(next() % 100) - 50;
        auto a = v, b = v, ea = v, eb = v;
        std::sort(ea.begin(), ea.end());
        std::sort(eb.begin(), eb.end(), std::greater<>{});
        heap_sort(a.begin(), a.end());
        heap_sort(b.begin(), b.end(), std::greater<>{});
        if (a != ea || b != eb) {
            std::cout << "FAIL on case " << t << '\n';
            return 1;
        }
    }
    std::cout << "passed: 5000 inputs, ascending and descending\n";
    return 0;
}
