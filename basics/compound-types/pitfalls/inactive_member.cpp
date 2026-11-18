// Pitfall, deliberately wrong: writes one member of a union and reads
// another. In C++ this is undefined behavior. It is not part of the build;
// compile it by hand to see that neither the compiler nor the sanitizers
// report it:
//   g++ -std=c++17 -Wall -Wextra -fsanitize=undefined pitfalls/inactive_member.cpp
#include <iostream>

union Number {
    int i;
    float f;
};

int main()
{
    Number n;
    n.i = 42;
    std::cout << "n.i = " << n.i << '\n';
    std::cout << "n.f = " << n.f << '\n';   // undefined behavior: f is not the active member
}
