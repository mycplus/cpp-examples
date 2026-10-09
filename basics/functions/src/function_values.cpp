// function_values.cpp - functions passed to other functions: a function
// pointer, a lambda, and std::function.
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

bool is_even(int n) { return n % 2 == 0; }

int apply_twice(const std::function<int(int)>& f, int x) { return f(f(x)); }

int main() {
    const std::vector<int> v{3, 8, 5, 12, 7, 6};

    // 1. A named function, passed as a pointer.
    std::cout << "even:      " << std::count_if(v.begin(), v.end(), is_even) << '\n';

    // 2. A lambda that captures a local variable.
    int limit = 6;
    std::cout << "above " << limit << ":   "
              << std::count_if(v.begin(), v.end(), [limit](int n) { return n > limit; }) << '\n';

    // 3. std::function stores any callable with a matching signature.
    std::cout << "twice +10: " << apply_twice([](int n) { return n + 10; }, 1) << '\n';
    std::cout << "twice *3:  " << apply_twice([](int n) { return n * 3; }, 2) << '\n';
}
