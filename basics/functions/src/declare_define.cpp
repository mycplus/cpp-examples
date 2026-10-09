// declare_define.cpp - a declaration lets main() call a function that is
// defined further down the file.
#include <iostream>

double circle_area(double radius);   // declaration (prototype)

int main() {
    std::cout << "area of r=2: " << circle_area(2.0) << '\n';
    std::cout << "area of r=0.5: " << circle_area(0.5) << '\n';
}

// definition: the declaration's signature plus a body
double circle_area(double radius) {
    constexpr double pi = 3.14159265358979;
    return pi * radius * radius;
}
