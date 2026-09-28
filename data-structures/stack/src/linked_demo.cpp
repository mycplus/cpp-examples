// linked_demo.cpp - LinkedStack<int> running the same sequence.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/linked_demo.cpp -o linked_demo
#include <iostream>
#include <stdexcept>

#include "linked_stack.hpp"

int main()
{
    LinkedStack<int> s;
    std::cout << "push 10 20 30\n";
    s.push(10);
    s.push(20);
    s.push(30);
    std::cout << "size=" << s.size() << " top=" << s.top() << '\n';
    while (!s.empty()) {
        std::cout << "pop " << s.top() << '\n';
        s.pop();
    }
    std::cout << "empty=" << std::boolalpha << s.empty() << '\n';
    try {
        s.pop();
        std::cout << "pop on empty: returned a value\n";
    } catch (const std::out_of_range& e) {
        std::cout << "pop on empty: underflow reported (" << e.what() << ")\n";
    }
}
