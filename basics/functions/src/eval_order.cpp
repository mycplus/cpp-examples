// eval_order.cpp - the order in which a call's arguments are evaluated is
// unspecified; GCC and Clang choose differently.
#include <iostream>

int next_ticket() {
    static int ticket = 0;
    return ++ticket;
}

void show(int first, int second) {
    std::cout << "first = " << first << ", second = " << second << '\n';
}

int main() {
    show(next_ticket(), next_ticket());

    // Portable: evaluate into named variables, in the order you need.
    const int a = next_ticket();
    const int b = next_ticket();
    show(a, b);
}
