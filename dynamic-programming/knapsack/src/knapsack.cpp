#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

struct Item {
    std::string name;
    int weight;
    long long value;
};

struct Selection {
    long long value = 0;
    std::vector<std::size_t> chosen;  // indices into the input, ascending
};

// 0/1 knapsack: best value within capacity, and which items give it.
// Throws std::invalid_argument for negative inputs or a total value that
// would overflow long long; std::bad_alloc if the table does not fit.
Selection knapsack(const std::vector<Item>& items, int capacity)
{
    if (capacity < 0)
        throw std::invalid_argument("capacity must be non-negative");

    long long total = 0;
    for (const Item& it : items) {
        if (it.weight < 0 || it.value < 0)
            throw std::invalid_argument("weights and values must be non-negative");
        if (it.value > std::numeric_limits<long long>::max() - total)
            throw std::invalid_argument("total value overflows long long");
        total += it.value;
    }

    const std::size_t n = items.size();
    const std::size_t cols = static_cast<std::size_t>(capacity) + 1;
    if (n > 0 && cols > std::numeric_limits<std::size_t>::max() / n)
        throw std::bad_alloc();

    std::vector<long long> best(cols, 0);
    std::vector<unsigned char> kept(n * cols, 0);

    for (std::size_t i = 0; i < n; ++i) {
        const int wt = items[i].weight;
        const long long val = items[i].value;
        for (int c = capacity; c >= wt; --c) {  // downwards: each item once
            if (best[c - wt] + val > best[c]) {
                best[c] = best[c - wt] + val;
                kept[i * cols + static_cast<std::size_t>(c)] = 1;
            }
        }
    }

    Selection result;
    result.value = best[static_cast<std::size_t>(capacity)];
    int c = capacity;
    for (std::size_t i = n; i-- > 0;) {
        if (kept[i * cols + static_cast<std::size_t>(c)]) {
            result.chosen.push_back(i);
            c -= items[i].weight;
        }
    }
    std::reverse(result.chosen.begin(), result.chosen.end());
    return result;
}

int main()
{
    const std::vector<Item> items{
        {"A", 10, 60}, {"B", 20, 100}, {"C", 30, 120}, {"D", 5, 50},
    };

    try {
        const Selection s = knapsack(items, 50);
        std::cout << "Best value " << s.value << " from:";
        for (std::size_t i : s.chosen)
            std::cout << ' ' << items[i].name;
        std::cout << '\n';
    } catch (const std::exception& e) {
        std::cerr << "knapsack failed: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
