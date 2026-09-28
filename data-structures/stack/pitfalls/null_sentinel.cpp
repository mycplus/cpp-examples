// null_sentinel.cpp - DO NOT COPY. A pop() that returns NULL to mean "empty".
// std::endl flushes each line so it survives the abort at the end.
#include <iostream>
#include <string>

template <typename T>
struct FixedStack {
    T items[10];
    int top = -1;
    void push(const T& v) { items[++top] = v; }
    T pop()
    {
        if (top == -1)
            return NULL;                  // the "empty" marker
        return items[top--];
    }
};

int main()
{
    FixedStack<int> a;
    a.push(0);
    std::cout << "int: pop after push(0) = " << a.pop() << std::endl;
    std::cout << "int: pop on empty      = " << a.pop() << std::endl;
    FixedStack<std::string> b;
    std::cout << "string: pop on empty   = " << b.pop() << std::endl;
}
