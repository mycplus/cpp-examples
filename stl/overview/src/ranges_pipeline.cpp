// ranges_pipeline.cpp - C++20 ranges: algorithms that take a whole container,
// and lazy views composed with |.
#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

int main()
{
    std::vector<int> v{5, 3, 8, 1, 9, 2, 7};

    std::ranges::sort(v);                         // no begin()/end() pair
    std::cout << "sorted:";
    for (int x : v) std::cout << ' ' << x;

    // Nothing is computed until the loop pulls values through the view.
    auto odd_squares = v | std::views::filter([](int x) { return x % 2 != 0; })
                         | std::views::transform([](int x) { return x * x; });
    std::cout << "\nodd squares:";
    for (int x : odd_squares) std::cout << ' ' << x;

    auto it = std::ranges::find(v, 8);
    std::cout << "\nfound 8 at index " << (it - v.begin()) << '\n';
}
