// overloading.cpp - compile-time polymorphism: the compiler picks an add()
// from the argument types before the program runs.
#include <iostream>
#include <string>

int         add(int a, int b)                 { return a + b; }
double      add(double a, double b)           { return a + b; }
std::string add(const std::string& a,
                const std::string& b)         { return a + b; }

int main() {
    std::cout << add(2, 3) << '\n';
    std::cout << add(2.5, 3.5) << '\n';
    std::cout << add(std::string("poly"), std::string("morphism")) << '\n';
}
