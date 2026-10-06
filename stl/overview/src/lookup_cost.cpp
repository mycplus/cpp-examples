// lookup_cost.cpp - how many element comparisons it takes to find one value
// among 1,000,000, depending on the container and the algorithm.
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

static long g_compares = 0;   // incremented by every comparison below

struct CountingLess {
    bool operator()(int a, int b) const { ++g_compares; return a < b; }
};
struct CountingEqual {
    bool operator()(int a, int b) const { ++g_compares; return a == b; }
};

// Every key in one bucket: what hashing degrades to when the hash is poor.
struct ConstantHash {
    std::size_t operator()(int) const { return 42; }
};

template <typename F>
long count(F&& lookup)
{
    g_compares = 0;
    lookup();
    return g_compares;
}

int main()
{
    constexpr int n = 1'000'000;
    const int target = 765'432;

    std::vector<int> values(n);
    for (int i = 0; i < n; ++i)
        values[i] = i;                      // already sorted

    std::set<int, CountingLess> tree(values.begin(), values.end());
    std::unordered_set<int, std::hash<int>, CountingEqual> table(values.begin(), values.end());

    long linear = count([&] {
        (void)std::find_if(values.begin(), values.end(),
                           [&](int x) { return CountingEqual{}(x, target); });
    });
    long binary = count([&] {
        (void)std::binary_search(values.begin(), values.end(), target, CountingLess{});
    });
    long set_find = count([&] { (void)tree.find(target); });
    long hash_find = count([&] { (void)table.find(target); });

    // Built with only 10,000 values: with one bucket, construction is quadratic.
    std::unordered_set<int, ConstantHash, CountingEqual> bad_table(values.begin(),
                                                                   values.begin() + 10'000);
    long bad_hash_find = count([&] { (void)bad_table.find(7'654); });

    std::cout << "searching " << n << " ints for " << target << "\n"
              << "std::find on vector:           " << linear << " comparisons\n"
              << "std::binary_search on vector:  " << binary << " comparisons\n"
              << "std::set::find:                " << set_find << " comparisons\n"
              << "std::unordered_set::find:      " << hash_find << " comparisons\n"
              << "same, constant hash, 10000 ints: " << bad_hash_find << " comparisons\n";
}
