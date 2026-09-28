// vector_growth.cpp - count std::vector reallocations over a million push_backs.
// The growth factor belongs to the standard library, so the count does too.
#include <cstddef>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v;
    std::size_t reallocations = 0;
    std::size_t last = v.capacity();
    for (int i = 0; i < 1'000'000; ++i) {
        v.push_back(i);
        if (v.capacity() != last) {
            ++reallocations;
            last = v.capacity();
        }
    }
    std::cout << "std::vector<int>: " << reallocations
              << " reallocations for 1000000 push_back, final capacity "
              << v.capacity() << '\n';
}
