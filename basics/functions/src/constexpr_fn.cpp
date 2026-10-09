// constexpr_fn.cpp - one function, evaluated at compile time or at run time.
#include <iostream>

constexpr long long factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

static_assert(factorial(5) == 120);     // checked by the compiler
constexpr long long table_size = factorial(10);

int main(int argc, char*[]) {
    const int n = argc + 5;              // known only when the program runs
    std::cout << n << "! = " << factorial(n) << " (run time)\n";
    std::cout << "10! = " << table_size << " (compile time)" << '\n';
}
