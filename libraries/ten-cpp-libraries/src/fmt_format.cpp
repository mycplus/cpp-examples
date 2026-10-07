// fmt_format.cpp - type-safe formatting with {fmt}, next to the same format
// string in C++20 std::format, which was standardised from {fmt}.
#include <fmt/core.h>
#include <fmt/format.h>

#include <format>
#include <iostream>
#include <string>

int main()
{
    const double pi = 3.14159265358979;
    const int count = 1234567;

    fmt::print("{:<10}|{:>10}|\n", "left", "right");
    fmt::print("{:^21}\n", "centred");
    fmt::print("pi to 3 places: {:.3f}, hex: {:#x}, binary: {:#b}, padded: {:>9}\n", pi, 255, 10, count);

    const std::string a = fmt::format("{:>8.2f}|{:08d}", pi, 42);
    const std::string b = std::format("{:>8.2f}|{:08d}", pi, 42);
    std::cout << a << '\n' << b << '\n'
              << "fmt and std::format agree: " << std::boolalpha << (a == b) << '\n';

    // A mismatched argument is rejected at compile time by both libraries:
    // fmt::format("{:d}", "text");   // error: invalid format specifier
}
