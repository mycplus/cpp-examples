// hanoi_cli.cpp - prints the moves for n disks, recursive or iterative.
// Usage: hanoi_cli N [--iterative]     (0 <= N <= 20)
#include <charconv>
#include <cstdio>
#include <iostream>
#include <string_view>
#include <system_error>

#include "hanoi.hpp"

namespace {
constexpr int max_printed_disks = 20;   // 1,048,575 lines

bool parse_disks(std::string_view text, int& out)
{
    int v = 0;
    const auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), v);
    if (ec != std::errc{} || end != text.data() + text.size() || v < 0 ||
        v > max_printed_disks)
        return false;
    out = v;
    return true;
}
}  // namespace

int main(int argc, char** argv)
{
    const bool iterative = argc == 3 && std::string_view(argv[2]) == "--iterative";
    int n = 0;
    if ((argc != 2 && !iterative) || !parse_disks(argv[1], n)) {
        std::cerr << "usage: " << (argc > 0 ? argv[0] : "hanoi_cli")
                  << " N [--iterative]   (N is 0 to " << max_printed_disks << ")\n";
        return 2;
    }

    const auto print = [](const hanoi::Move& m) {
        std::cout << "Move disk " << m.disk << " from " << m.from << " to " << m.to << '\n';
    };
    if (iterative)
        hanoi::solve_iterative(n, 'A', 'C', 'B', print);
    else
        hanoi::solve(n, 'A', 'C', 'B', print);
    std::cout << n << " disks: " << *hanoi::move_count(n) << " moves\n";
}
