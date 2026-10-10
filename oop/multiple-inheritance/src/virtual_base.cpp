// virtual_base.cpp - the diamond with virtual inheritance: one shared Device,
// constructed by the most-derived class.
#include <iostream>

struct Device {
    int id;
    explicit Device(int i) : id(i) { std::cout << "  Device(" << i << ") constructed\n"; }
};

struct Scanner : virtual Device {
    Scanner() : Device(1) {}               // ignored when Scanner is a base of Copier
};

struct Printer : virtual Device {
    Printer() : Device(2) {}               // ignored as well
};

struct Copier : Scanner, Printer {
    Copier() : Device(3) {}                // the most-derived class constructs Device
};

int main() {
    std::cout << "building a Copier:\n";
    Copier c;
    std::cout << "one Device, id " << c.id << '\n';
    std::cout << "same object: " << std::boolalpha
              << (static_cast<Device*>(static_cast<Scanner*>(&c)) ==
                  static_cast<Device*>(static_cast<Printer*>(&c))) << '\n';
    std::cout << "sizeof(Copier) = " << sizeof(Copier) << '\n';

    std::cout << "building a Scanner on its own:\n";
    Scanner s;
    std::cout << "its Device id: " << s.id << '\n';
}
