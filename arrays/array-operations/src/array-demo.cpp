// array-demo.cpp - the array operations using standard containers.
//
//   c++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/array-demo.cpp -o array-demo
#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "array_ops.hpp"

namespace {

template <typename Container>
void print(const Container &c)
{
    std::cout << '[';
    bool first = true;
    for (const auto &v : c) {
        if (!first) std::cout << ", ";
        std::cout << v;
        first = false;
    }
    std::cout << "]\n";
}

} // namespace

int main()
{
    // std::array is a fixed-size array that knows its own length. Unlike a
    // built-in array it does not decay to a pointer when passed by value
    // or reference, so size() is available wherever the object is.
    std::array<int, 5> fixed = { 10, 20, 30, 40, 50 };

    std::cout << "-- traverse\n";
    print(fixed);

    std::cout << "\n-- access and update\n";
    std::cout << "fixed[2] is " << fixed[2] << '\n';
    fixed[2] = 35;
    std::cout << "after fixed[2] = 35: ";
    print(fixed);
    // operator[] does not check the index; at() throws std::out_of_range.
    try {
        (void)fixed.at(9);
    } catch (const std::out_of_range &e) {
        std::cout << "fixed.at(9) threw std::out_of_range\n";
    }

    std::cout << "\n-- linear search\n";
    if (const auto at = mycplus::find_index(fixed, 40))
        std::cout << "40 found at index " << *at << '\n';
    if (!mycplus::find_index(fixed, 99))
        std::cout << "99 not found\n";

    // std::vector owns a resizable buffer, so insertion and deletion are
    // member functions rather than manual shifting.
    std::vector<int> dyn(fixed.begin(), fixed.end());

    std::cout << "\n-- insertion\n";
    dyn.insert(dyn.begin() + 2, 25);
    std::cout << "insert 25 at index 2: ";
    print(dyn);
    dyn.push_back(60);
    std::cout << "append 60:            ";
    print(dyn);

    std::cout << "\n-- deletion\n";
    dyn.erase(dyn.begin());
    std::cout << "erase index 0:        ";
    print(dyn);

    std::cout << "\n-- array size\n";
    std::cout << "fixed.size() = " << fixed.size()
              << ", sizeof fixed = " << sizeof fixed << " bytes\n";
    std::cout << "dyn.size() = " << dyn.size()
              << ", dyn.capacity() >= size() = "
              << (dyn.capacity() >= dyn.size() ? "true" : "false") << '\n';

    std::cout << "\n-- multi-dimensional\n";
    // An array of arrays is contiguous and keeps its shape in the type.
    std::array<std::array<int, 3>, 2> grid = { { { 1, 2, 3 }, { 4, 5, 6 } } };
    for (const auto &row : grid) {
        for (const int v : row)
            std::cout << ' ' << v;
        std::cout << '\n';
    }
    grid[1][2] = 60;
    std::cout << "after grid[1][2] = 60: " << grid[1][2] << '\n';
    std::cout << "rows = " << grid.size() << ", cols = " << grid[0].size() << '\n';

    return 0;
}
