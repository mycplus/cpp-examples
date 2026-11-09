// bst_demo.cpp - the article's tree, built with BinarySearchTree<int>.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic -I../include bst_demo.cpp -o bst_demo
#include "binary_search_tree.hpp"

#include <iomanip>
#include <iostream>
#include <string>

namespace {

template <typename Walk>
void print(const std::string& label, Walk walk)
{
    std::cout << std::left << std::setw(13) << label;
    bool first = true;
    walk([&first](int key) {
        std::cout << (first ? "" : " ") << key;
        first = false;
    });
    std::cout << '\n';
}

void show(const BinarySearchTree<int>& t, bool depth_first)
{
    if (depth_first) {
        print("preorder:", [&t](auto f) { t.for_each_preorder(f); });
        print("inorder:", [&t](auto f) { t.for_each_inorder(f); });
        print("postorder:", [&t](auto f) { t.for_each_postorder(f); });
    } else {
        print("inorder:", [&t](auto f) { t.for_each_inorder(f); });
    }
    print("level order:", [&t](auto f) { t.for_each_level_order(f); });
}

}  // namespace

int main()
{
    BinarySearchTree<int> tree;
    for (int key : {50, 30, 70, 20, 40, 60, 80, 35, 45, 65})
        tree.insert(key);
    std::cout << "inserted:    50 30 70 20 40 60 80 35 45 65\n";
    show(tree, true);
    std::cout << "size " << tree.size() << ", height " << tree.height() << '\n';
    std::cout << "contains 45: " << (tree.contains(45) ? "yes" : "no")
              << ", contains 55: " << (tree.contains(55) ? "yes" : "no") << '\n';
    std::cout << "insert 40 again: " << tree.insert(40) << '\n';

    tree.erase(30);
    std::cout << "\nremove 30 (two children)\n";
    show(tree, false);

    tree.erase(50);
    tree.erase(20);
    std::cout << "\nremove 50 (root), 20 (leaf)\n";
    show(tree, false);
    std::cout << "size " << tree.size() << ", height " << tree.height() << '\n';
    std::cout << "remove 99: " << (tree.erase(99) ? "removed" : "not found") << '\n';
}
