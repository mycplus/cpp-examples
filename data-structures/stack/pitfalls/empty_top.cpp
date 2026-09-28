// empty_top.cpp - DO NOT COPY. std::stack::top() after the last pop().
// top() calls back() on the underlying container, and back() on an empty
// container is undefined behaviour. Build with -D_GLIBCXX_ASSERTIONS to
// turn it into a checked failure on libstdc++.
#include <iostream>
#include <stack>
#include <vector>

int main()
{
    std::stack<int, std::vector<int>> s;
    s.push(42);
    s.pop();                                   // s is now empty
    std::cout << "empty() = " << std::boolalpha << s.empty()
              << ", top() = " << s.top() << '\n';
}
