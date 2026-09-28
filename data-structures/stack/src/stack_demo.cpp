// stack_demo.cpp - ArrayStack<int> in use, and a bracket checker on std::stack.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/stack_demo.cpp -o stack_demo
#include <iostream>
#include <stack>
#include <string_view>

#include "array_stack.hpp"

bool balanced(std::string_view text)
{
    std::stack<char> open;
    for (char c : text) {
        switch (c) {
        case '(': case '[': case '{':
            open.push(c);
            break;
        case ')': case ']': case '}': {
            const char want = c == ')' ? '(' : c == ']' ? '[' : '{';
            if (open.empty() || open.top() != want)
                return false;
            open.pop();
            break;
        }
        default:
            break;
        }
    }
    return open.empty();
}

int main()
{
    ArrayStack<int> s;
    std::cout << "push 10 20 30\n";
    s.push(10);
    s.push(20);
    s.push(30);
    std::cout << "size=" << s.size() << " top=" << s.top() << '\n';
    while (auto v = s.try_pop())
        std::cout << "pop " << *v << '\n';
    std::cout << "empty=" << std::boolalpha << s.empty() << '\n';
    std::cout << "pop on empty: "
              << (s.try_pop() ? "returned a value" : "underflow reported") << '\n';

    for (std::string_view t : {"{[()()]}", "([)]", "((", "())", ""})
        std::cout << "balanced(\"" << t << "\") = " << balanced(t) << '\n';
}
