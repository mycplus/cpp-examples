// algorithm_counts.cpp - the C++ standard specifies how many operations an
// algorithm may perform. This counts predicate and comparison calls.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

int main()
{
    std::vector<int> v(1000);
    std::mt19937 gen(7);                          // fixed sequence on every library
    for (int& x : v) x = static_cast<int>(gen() % 10000);

    long calls = 0;
    auto big = [&](int x) { ++calls; return x > 5000; };

    calls = 0;
    auto n_big = std::count_if(v.begin(), v.end(), big);
    std::cout << "count_if: " << n_big << " matches, predicate called " << calls
              << " times for " << v.size() << " elements\n";

    calls = 0;
    auto it = std::find_if(v.begin(), v.end(), big);
    std::cout << "find_if:  first match at index " << (it - v.begin())
              << ", predicate called " << calls << " times\n";

    long compares = 0;
    std::sort(v.begin(), v.end(), [&](int a, int b) { ++compares; return a < b; });
    const double n = static_cast<double>(v.size());
    const double n_log_n = n * std::log2(n);
    std::cout << "sort:     " << compares << " comparisons; N log2 N = "
              << static_cast<long>(n_log_n) << '\n';
}
