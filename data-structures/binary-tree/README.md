# Binary Search Tree in C++

[![binary-tree](https://github.com/mycplus/cpp-examples/actions/workflows/binary-tree.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/binary-tree.yml)

Companion code for [Binary Trees in C, C++, Java, Python and C#](https://www.mycplus.com/computer-science/data-structures/binary-trees/) on MYCPLUS.

| File | What it is |
| --- | --- |
| `include/binary_search_tree.hpp` | `BinarySearchTree<T, Compare>`: nodes owned by `std::unique_ptr`, insert, search and erase without recursion, the four traversals with an explicit stack or queue, and a destructor that does not recurse |
| `src/bst_demo.cpp` | The article's demo tree; prints the same output as the C, Java, Python and C# versions |
| `src/set_vs_bst.cpp` | Inserts and looks up 20,000 keys in random and sorted order, in `BinarySearchTree` and in `std::set` |
| `pitfalls/less_equal.cpp` | `std::less_equal` as the comparator: a key is stored twice and then cannot be found |
| `tests/` | Unit and model-based tests, a small-stack destruction test, and the demo's expected output |

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## What the build checks

- Compiles with GCC and Clang under C++17 and C++20 with `-Wall -Wextra -pedantic -Werror`, and with MSVC under `/W4 /WX /permissive-`.
- The demo tree's four traversal orders, height and size, and every removal case: a leaf, a node with one child on either side, a node with two children and the root.
- An empty tree, a one-node tree keyed `INT_MIN`, move construction and move assignment, and a `std::string` tree ordered by `std::greater`.
- 200,000 random inserts, erases and searches agree with `std::set<int>`, including `INT_MIN` and `INT_MAX`; size and in-order output are compared every 1,000 operations.
- A 20,000-node chain is destroyed on a thread with a 64 KiB stack (Linux and macOS only).
- `bst_demo` prints exactly the output shown in the article, and `less_equal` reports `contains 40: no`.
- The tests, the demo and the pitfall run clean under AddressSanitizer and UndefinedBehaviorSanitizer.

The build does **not** run `set_vs_bst`: its timings depend on the machine.
