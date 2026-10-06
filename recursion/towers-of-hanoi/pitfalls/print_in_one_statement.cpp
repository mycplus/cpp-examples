// print_in_one_statement.cpp - DO NOT COPY. hanoi() prints the moves, and
// main() calls it in the middle of the statement that prints the total.
// Since C++17 the operands of << are evaluated left to right, so
// "3 disks: " is written before hanoi() runs and prints the moves.
#include <cstdint>
#include <iostream>

std::uint64_t hanoi(int n, char source, char target, char spare)
{
    if (n <= 0)
        return 0;
    std::uint64_t moves = hanoi(n - 1, source, spare, target);
    std::cout << "Move disk " << n << " from " << source << " to " << target << '\n';
    ++moves;
    moves += hanoi(n - 1, spare, target, source);
    return moves;
}

int main()
{
    const int n = 3;
    std::cout << n << " disks: " << hanoi(n, 'A', 'C', 'B') << " moves\n";
}
