// diamond.cpp - the diamond without virtual inheritance: a Copier contains
// two separate Device objects.
#include <iostream>

struct Device {
    int id;
    explicit Device(int i) : id(i) { std::cout << "  Device(" << i << ") constructed\n"; }
};

struct Scanner : Device {
    Scanner() : Device(1) {}
};

struct Printer : Device {
    Printer() : Device(2) {}
};

struct Copier : Scanner, Printer {};

int main() {
    std::cout << "building a Copier:\n";
    Copier c;
    std::cout << "Scanner's Device id: " << c.Scanner::id << '\n';
    std::cout << "Printer's Device id: " << c.Printer::id << '\n';
    std::cout << "sizeof(Device) = " << sizeof(Device) << ", sizeof(Copier) = " << sizeof(Copier) << '\n';
}
