// test_library_sorts.cpp - checks the claims the article makes about the
// standard library: std::stable_sort keeps equal keys in order on 5,000
// random inputs, and std::sort, std::ranges::sort and std::stable_sort
// agree with each other on every input.
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

int main()
{
    std::uint64_t xs = 0x9E3779B97F4A7C15ULL;
    auto next = [&xs] { xs ^= xs << 13; xs ^= xs >> 7; xs ^= xs << 17; return xs; };
    auto by_key = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.first < b.first;
    };

    for (int t = 0; t < 5000; ++t) {
        std::vector<std::pair<int, int>> v(next() % 200);        // (key, position)
        for (std::size_t i = 0; i < v.size(); ++i)
            v[i] = {static_cast<int>(next() % 8), static_cast<int>(i)};

        auto s = v, r = v, st = v;
        std::sort(s.begin(), s.end());          // full pair order: the reference
        std::ranges::sort(r);
        std::stable_sort(st.begin(), st.end(), by_key);

        // stable by key == sorted by (key, original position)
        if (r != s || st != s) {
            std::cout << "FAIL on case " << t << '\n';
            return 1;
        }
    }
    std::cout << "passed: 5000 random inputs\n";
    return 0;
}
