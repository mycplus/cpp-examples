// hello.cpp - no exceptions anywhere: no try, no throw, no C++ library
// calls that can throw. Used to check which runtime DLLs such a program needs.
#include <cstdio>

int main()
{
    std::puts("hello");
}
