// test_bst.cpp - tests for binary_search_tree.hpp.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic -I../include test_bst.cpp -o test_bst
#include "binary_search_tree.hpp"

#include <climits>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <set>
#include <string>
#include <vector>

namespace {

int failures = 0;

void check(bool ok, int line, const char* what)
{
    if (!ok) {
        std::fprintf(stderr, "FAIL test_bst.cpp:%d: %s\n", line, what);
        ++failures;
    }
}
#define CHECK(cond) check((cond), __LINE__, #cond)

template <typename T, typename C, typename Walk>
std::vector<T> collect(const BinarySearchTree<T, C>& t, Walk walk)
{
    std::vector<T> out;
    walk(t, [&out](const T& k) { out.push_back(k); });
    return out;
}

template <typename T, typename C>
std::vector<T> inorder(const BinarySearchTree<T, C>& t)
{
    return collect<T>(t, [](const auto& tree, auto f) { tree.for_each_inorder(f); });
}

void test_demo_tree()
{
    BinarySearchTree<int> t;
    for (int k : {50, 30, 70, 20, 40, 60, 80, 35, 45, 65})
        CHECK(t.insert(k));
    CHECK(!t.insert(40));
    using V = std::vector<int>;
    CHECK(collect<int>(t, [](const auto& tr, auto f) { tr.for_each_preorder(f); })
          == V({50, 30, 20, 40, 35, 45, 70, 60, 65, 80}));
    CHECK(inorder(t) == V({20, 30, 35, 40, 45, 50, 60, 65, 70, 80}));
    CHECK(collect<int>(t, [](const auto& tr, auto f) { tr.for_each_postorder(f); })
          == V({20, 35, 45, 40, 30, 65, 60, 80, 70, 50}));
    CHECK(collect<int>(t, [](const auto& tr, auto f) { tr.for_each_level_order(f); })
          == V({50, 30, 70, 20, 40, 60, 80, 35, 45, 65}));
    CHECK(t.height() == 3 && t.size() == 10);

    CHECK(t.erase(65));                   // leaf
    CHECK(t.erase(40));                   // two children
    CHECK(t.erase(45));                   // left child only
    CHECK(t.erase(70));                   // two children
    CHECK(t.erase(80));                   // left child only
    CHECK(t.erase(30));                   // right child only
    CHECK(t.erase(50));                   // root, two children
    CHECK(!t.erase(50));
    CHECK(inorder(t) == V({20, 35, 60}));
    CHECK(t.size() == 3);
}

void test_empty_move_and_comparator()
{
    BinarySearchTree<int> e;
    CHECK(e.empty() && e.size() == 0 && e.height() == -1);
    CHECK(!e.contains(0) && !e.erase(0) && inorder(e).empty());

    BinarySearchTree<std::string, std::greater<std::string>> words;
    for (const char* w : {"pear", "apple", "fig", "kiwi", "apple"})
        words.insert(w);
    CHECK(inorder(words) == std::vector<std::string>({"pear", "kiwi", "fig", "apple"}));

    BinarySearchTree<std::string, std::greater<std::string>> moved(std::move(words));
    CHECK(words.empty() && moved.size() == 4 && moved.contains("fig"));
    words = std::move(moved);
    CHECK(moved.empty() && words.size() == 4 && words.contains("kiwi"));

    BinarySearchTree<int> one;
    CHECK(one.insert(INT_MIN) && one.height() == 0);
    CHECK(one.erase(INT_MIN) && one.empty());
}

// 200,000 random operations against std::set.
void test_against_std_set()
{
    std::uint64_t x = 2463534242ULL;
    BinarySearchTree<int> t;
    std::set<int> model;
    for (int op = 0; op < 200000; ++op) {
        x ^= x << 13; x ^= x >> 7; x ^= x << 17;
        int i = static_cast<int>(x % 2000);
        int key = i == 0 ? INT_MIN : i == 1999 ? INT_MAX : i * 1000003 - 1000000000;
        switch ((x >> 32) % 3) {
        case 0: CHECK(t.insert(key) == model.insert(key).second); break;
        case 1: CHECK(t.erase(key) == (model.erase(key) == 1)); break;
        default: CHECK(t.contains(key) == (model.count(key) == 1));
        }
        if (op % 1000 == 0) {
            CHECK(t.size() == model.size());
            CHECK(inorder(t) == std::vector<int>(model.begin(), model.end()));
        }
    }
}

}  // namespace

int main()
{
    test_demo_tree();
    test_empty_move_and_comparator();
    test_against_std_set();
    if (failures) {
        std::fprintf(stderr, "%d check(s) failed\n", failures);
        return 1;
    }
    std::puts("all binary tree tests passed");
}
