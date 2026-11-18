// declarations.cpp - compound types combine. Read a declaration from the name
// outward: right first (arrays, function parameters), then left (pointers,
// references), and parentheses change the order.
#include <cstddef>
#include <iostream>
#include <type_traits>

int twice(double x) { return static_cast<int>(x * 2); }

int counter = 0;
int& next_counter() { return ++counter; }

int main()
{
    int a = 1, b = 2, c = 3;
    int row[3] = {7, 8, 9};

    int* pointers[3] = {&a, &b, &c};   // array of 3 pointers to int
    int (*to_row)[3] = &row;           // pointer to an array of 3 int
    int (*fn)(double) = &twice;        // pointer to a function taking double, returning int
    int& (*counter_fn)() = &next_counter;   // pointer to a function returning int&

    static_assert(std::is_same_v<decltype(pointers), int* [3]>);
    static_assert(std::is_same_v<decltype(to_row), int (*)[3]>);
    static_assert(std::is_same_v<decltype(fn), int (*)(double)>);

    std::cout << "sizeof(pointers) = " << sizeof(pointers) << " (three pointers)\n";
    std::cout << "sizeof(to_row)   = " << sizeof(to_row) << " (one pointer)\n";
    std::cout << "*pointers[1] = " << *pointers[1] << ", (*to_row)[1] = " << (*to_row)[1] << '\n';
    std::cout << "fn(2.6) = " << fn(2.6) << '\n';
    counter_fn() += 100;                 // the call returns a reference, so it can be assigned
    std::cout << "counter = " << counter << '\n';

    // Type aliases make the same types readable.
    using Row = int[3];
    using Transform = int (*)(double);
    Row* same_as_to_row = &row;
    Transform same_as_fn = twice;       // a function name decays to a pointer, like an array
    static_assert(std::is_same_v<Row*, decltype(to_row)>);
    std::cout << "via aliases: " << (*same_as_to_row)[2] << ", " << same_as_fn(1.0) << '\n';
}
