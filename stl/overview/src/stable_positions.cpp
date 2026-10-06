// stable_positions.cpp - three ways to keep track of an element while a
// vector grows: an index, reserve(), or a node-based container.
#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

int main()
{
    // 1. An index survives reallocation; an iterator does not.
    std::vector<int> scores{70, 85, 90};
    std::size_t best = 2;
    scores.push_back(95);
    std::cout << "index:   best score = " << scores[best] << '\n';

    // 2. reserve() up front: no reallocation while size() <= capacity().
    std::vector<int> reserved{70, 85, 90};
    reserved.reserve(8);
    auto it = reserved.begin() + 2;
    const auto* data_before = reserved.data();
    reserved.push_back(95);
    std::cout << "reserve: best score = " << *it << ", storage moved: "
              << (reserved.data() != data_before ? "yes" : "no") << '\n';

    // 3. std::list never moves its elements; iterators stay valid until erased.
    std::list<int> nodes{70, 85, 90};
    auto node = std::next(nodes.begin(), 2);
    for (int i = 0; i < 1000; ++i)
        nodes.push_back(i);
    std::cout << "list:    best score = " << *node << '\n';
}
