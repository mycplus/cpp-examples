// list_sort_moves.cpp - list::sort reorders nodes; std::sort on a vector moves
// values. Counts element copies and moves, and checks what an iterator held
// across the sort refers to afterwards.
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <random>
#include <vector>

static long g_copies = 0, g_moves = 0;

struct Record {
    int key;
    explicit Record(int k) : key(k) {}
    Record(const Record& o) : key(o.key) { ++g_copies; }
    Record(Record&& o) noexcept : key(o.key) { ++g_moves; }
    Record& operator=(const Record& o) { key = o.key; ++g_copies; return *this; }
    Record& operator=(Record&& o) noexcept { key = o.key; ++g_moves; return *this; }
};

static bool by_key(const Record& a, const Record& b) { return a.key < b.key; }

int main()
{
    constexpr int n = 1000;
    std::mt19937 gen(42);                    // the standard fixes mt19937's output sequence
    std::vector<int> keys(n);
    for (int& k : keys) k = static_cast<int>(gen() % 100000);

    std::list<Record> lst;
    std::vector<Record> vec;
    vec.reserve(n);
    for (int k : keys) { lst.emplace_back(k); vec.emplace_back(k); }

    auto lit = lst.begin();                  // first element, before sorting
    auto vit = vec.begin();
    const int first_key = lit->key;

    g_copies = g_moves = 0;
    lst.sort(by_key);
    std::cout << "list::sort   copies " << g_copies << ", moves " << g_moves << '\n';

    g_copies = g_moves = 0;
    std::sort(vec.begin(), vec.end(), by_key);
    std::cout << "std::sort    copies " << g_copies << ", moves " << g_moves << '\n';

    std::cout << "key before sort:            " << first_key << '\n'
              << "list iterator now refers to " << lit->key
              << " (same element, new position "
              << std::distance(lst.begin(), lit) << ")\n"
              << "vector iterator refers to   " << vit->key
              << " (whatever value is now at index 0)\n";
}
