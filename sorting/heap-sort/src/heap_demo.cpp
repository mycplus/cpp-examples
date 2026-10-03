// heap_demo.cpp - the generic heap sort, and the standard library's heap tools
#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include "heap_sort.hpp"

template <class Range>
void print(const char* label, const Range& r)
{
    std::cout << label;
    for (const auto& x : r) std::cout << ' ' << x;
    std::cout << '\n';
}

int main()
{
    std::vector<int> v{29, 10, 14, 37, 13, 5, 41, 22};
    heap_sort(v.begin(), v.end());
    print("heap_sort:        ", v);

    heap_sort(v.begin(), v.end(), std::greater<>{});
    print("descending:       ", v);

    std::vector<std::string> words{"pear", "fig", "apple", "kiwi", "date"};
    heap_sort(words.begin(), words.end());
    print("strings:          ", words);

    std::vector<int> w{29, 10, 14, 37, 13, 5, 41, 22};
    std::make_heap(w.begin(), w.end());               // the same max-heap layout
    print("std::make_heap:   ", w);
    std::sort_heap(w.begin(), w.end());
    print("std::sort_heap:   ", w);

    std::vector<int> k{29, 10, 14, 37, 13, 5, 41, 22};
    std::partial_sort(k.begin(), k.begin() + 3, k.end());   // three smallest, sorted
    std::cout << "partial_sort, 3:   " << k[0] << ' ' << k[1] << ' ' << k[2] << '\n';

    std::vector<int> q{29, 10, 14, 37, 13, 5, 41, 22};
    std::priority_queue<int> pq(q.begin(), q.end());      // a max-heap underneath
    std::cout << "priority_queue:   ";
    while (!pq.empty()) { std::cout << ' ' << pq.top(); pq.pop(); }
    std::cout << '\n';
    return 0;
}
