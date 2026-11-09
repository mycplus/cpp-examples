// less_equal.cpp - a BST given <= as its comparator.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic -I../include less_equal.cpp -o less_equal
//
// The tree decides that two keys are equivalent when neither compares
// before the other. With <=, a key always compares before itself, so the
// tree never sees a duplicate: it stores 40 twice, and then a search for
// 40 walks past both copies and reports it missing.
#include "binary_search_tree.hpp"

#include <functional>
#include <iostream>

int main()
{
    BinarySearchTree<int, std::less_equal<int>> tree;
    for (int key : {50, 30, 70, 40, 40})
        tree.insert(key);
    std::cout << "size after inserting 50 30 70 40 40: " << tree.size() << '\n';
    std::cout << "contains 40: " << (tree.contains(40) ? "yes" : "no") << '\n';
    std::cout << "erase 40: " << (tree.erase(40) ? "removed" : "not found") << '\n';
    std::cout << "in order:";
    tree.for_each_inorder([](int k) { std::cout << ' ' << k; });
    std::cout << '\n';
}
