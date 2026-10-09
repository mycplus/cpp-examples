// overloading.cpp - the compiler picks an overload from the argument types.
#include <iostream>
#include <string_view>

void describe(int value)              { std::cout << "int:         " << value << '\n'; }
void describe(double value)           { std::cout << "double:      " << value << '\n'; }
void describe(std::string_view value) { std::cout << "string_view: " << value << '\n'; }

int main() {
    describe(42);        // exact match: int
    describe(4.5);       // exact match: double
    describe('A');       // char is promoted to int
    describe(2.5f);      // float is promoted to double
    describe(true);      // bool is promoted to int
    describe("text");    // const char* converts to string_view
}
