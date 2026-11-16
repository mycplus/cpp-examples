// linear_search.cpp - linear search in C++17: a generic function, the
// standard library's std::find and std::find_if, move-to-front with
// std::rotate, and std::lower_bound for sorted data.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic linear_search.cpp -o linear_search
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <optional>
#include <string>
#include <vector>

// Position of the first element equal to value in any container that can be
// walked from begin() to end(), or std::nullopt.
template <class Container, class T>
std::optional<std::size_t> find_index(const Container& c, const T& value)
{
    std::size_t i = 0;
    for (const auto& element : c) {
        if (element == value)
            return i;
        ++i;
    }
    return std::nullopt;
}

// Moves the first element equal to value to the front and returns where it
// was, or std::nullopt. std::rotate shifts the elements before it back by one.
template <class T>
std::optional<std::size_t> move_to_front(std::vector<T>& v, const T& value)
{
    const auto it = std::find(v.begin(), v.end(), value);
    if (it == v.end())
        return std::nullopt;
    const auto at = static_cast<std::size_t>(it - v.begin());
    std::rotate(v.begin(), it, it + 1);
    return at;
}

struct Product {
    std::string name;
    double price;
};

namespace {

void print(const char* label, const std::vector<int>& v)
{
    std::cout << label;
    for (const int x : v)
        std::cout << ' ' << x;
    std::cout << '\n';
}

void report(const char* what, std::optional<std::size_t> at)
{
    std::cout << what << ": ";
    if (at)
        std::cout << "index " << *at << '\n';
    else
        std::cout << "not found\n";
}

}  // namespace

int main()
{
    const std::vector<int> numbers{42, 7, 19, 73, 7, 0, -5, 88};
    report("find_index 73 in a vector", find_index(numbers, 73));
    report("find_index 50 in a vector", find_index(numbers, 50));

    const std::list<std::string> words{"red", "green", "blue"};
    report("find_index \"blue\" in a list", find_index(words, std::string("blue")));

    // std::find returns an iterator: end() means "not found".
    const auto it = std::find(numbers.begin(), numbers.end(), 7);
    std::cout << "std::find 7: index " << std::distance(numbers.begin(), it) << '\n';

    // std::find_if takes a condition instead of a value.
    const std::vector<Product> shelf{{"pen", 1.25}, {"notebook", 3.5}, {"stapler", 7.0}};
    const auto p = std::find_if(shelf.begin(), shelf.end(),
                                [](const Product& x) { return x.price > 3.0; });
    if (p != shelf.end())
        std::cout << "first product over 3.00: " << p->name << '\n';

    std::vector<int> list{1, 2, 3, 4, 5, 6};
    for (const int key : {5, 5, 6, 5}) {
        const auto was = move_to_front(list, key);
        std::cout << "find " << key << " (was at " << (was ? static_cast<long>(*was) : -1L) << "):";
        print("", list);
    }

    // For sorted data, std::lower_bound finds the first element >= value.
    const std::vector<int> sorted{-5, 0, 7, 7, 19, 42, 73, 88};
    const auto lb = std::lower_bound(sorted.begin(), sorted.end(), 7);
    std::cout << "std::lower_bound 7 in the sorted copy: index " << (lb - sorted.begin()) << '\n';
    return 0;
}
