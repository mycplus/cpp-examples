// base_order.cpp - bases are constructed in declaration order, whatever order
// the constructor's initializer list uses.
#include <iostream>

struct Logger  { Logger()  { std::cout << "  Logger\n"; } };
struct Network { Network() { std::cout << "  Network\n"; } };
struct Storage { Storage() { std::cout << "  Storage\n"; } };

struct Service : Storage, Logger, Network {
    Service() : Network(), Logger(), Storage() { std::cout << "  Service body\n"; }
};

int main() {
    std::cout << "construction order:\n";
    Service s;
}
