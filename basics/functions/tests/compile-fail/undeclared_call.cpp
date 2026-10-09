// undeclared_call.cpp - must not compile: area() is used before any
// declaration of it.
#include <iostream>

int main() {
    std::cout << area(3.0) << '\n';
}

double area(double side) { return side * side; }
