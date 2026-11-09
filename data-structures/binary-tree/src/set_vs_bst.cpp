// set_vs_bst.cpp - an unbalanced BST against std::set on sorted and random keys.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic -O2 -I../include set_vs_bst.cpp -o set_vs_bst
//
// Inserts n keys, then looks every one of them up, and reports the median of
// five runs. libstdc++ implements std::set as a red-black tree, which keeps
// its height logarithmic whatever the insertion order.
#include "binary_search_tree.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <numeric>
#include <set>
#include <vector>

namespace {

std::uint64_t state = 88172645463325252ULL;   // xorshift64, as in the C programs

std::uint64_t next_rand()
{
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

template <typename Set>
double run_ms(const std::vector<int>& keys, std::size_t& found)
{
    auto t0 = std::chrono::steady_clock::now();
    Set s;
    for (int k : keys)
        s.insert(k);
    found = 0;
    for (int k : keys)
        found += s.count(k);
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

// BinarySearchTree has contains(), std::set has count(); adapt one to the other.
struct Bst : BinarySearchTree<int> {
    std::size_t count(int k) const { return contains(k) ? 1 : 0; }
};

template <typename Set>
double median_ms(const std::vector<int>& keys)
{
    std::vector<double> t;
    for (int i = 0; i < 5; ++i) {
        std::size_t found = 0;
        t.push_back(run_ms<Set>(keys, found));
        if (found != keys.size()) {
            std::fprintf(stderr, "lookup failed\n");
            std::exit(1);
        }
    }
    std::sort(t.begin(), t.end());
    return t[2];
}

}  // namespace

int main()
{
    const std::size_t n = 20000;
    std::vector<int> sorted(n);
    std::iota(sorted.begin(), sorted.end(), 0);
    std::vector<int> random = sorted;
    for (std::size_t i = n; i > 1; --i)
        std::swap(random[i - 1], random[next_rand() % i]);

    Bst probe;
    for (int k : sorted)
        probe.insert(k);
    long sorted_height = probe.height();
    probe.clear();
    for (int k : random)
        probe.insert(k);
    long random_height = probe.height();

    std::printf("n = %zu: insert every key, then look every key up (median of 5, ms)\n", n);
    std::printf("%-8s %12s %18s %12s\n", "order", "BST height", "BinarySearchTree", "std::set");
    std::printf("%-8s %12ld %18.1f %12.1f\n", "random", random_height,
                median_ms<Bst>(random), median_ms<std::set<int>>(random));
    std::printf("%-8s %12ld %18.1f %12.1f\n", "sorted", sorted_height,
                median_ms<Bst>(sorted), median_ms<std::set<int>>(sorted));
}
