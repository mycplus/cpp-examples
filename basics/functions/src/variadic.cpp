// variadic.cpp - type-checked ways to accept any number of arguments.
#include <initializer_list>
#include <iostream>
#include <string>

// All arguments of one type: std::initializer_list.
double average(std::initializer_list<double> values) {
    double total = 0;
    for (double v : values) total += v;
    return values.size() ? total / static_cast<double>(values.size()) : 0.0;
}

// Arguments of any types: a variadic template with a fold expression (C++17).
template <typename... Args>
void print_all(const Args&... args) {
    ((std::cout << args << ' '), ...);
    std::cout << "(" << sizeof...(args) << " arguments)\n";
}

template <typename... Nums>
auto sum(Nums... nums) {
    return (nums + ... + 0);
}

int main() {
    std::cout << "average: " << average({5, 7, 6, 8}) << '\n';
    print_all(1, 2.5, 'c', std::string("four"));
    print_all();
    std::cout << "sum: " << sum(1, 2, 3, 4) << '\n';
}
