// towers_of_hanoi.cpp - the recursive Towers of Hanoi solution in C++17.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic towers_of_hanoi.cpp -o towers_of_hanoi
#include <cstdint>
#include <iostream>

// Moves n disks from source to target, using spare as the third peg,
// prints each move, and returns the number of moves made.
std::uint64_t hanoi(int n, char source, char target, char spare)
{
    if (n <= 0)
        return 0;                                    // nothing to move

    std::uint64_t moves = hanoi(n - 1, source, spare, target);
    std::cout << "Move disk " << n << " from " << source << " to " << target << '\n';
    ++moves;
    moves += hanoi(n - 1, spare, target, source);
    return moves;
}

int main()
{
    const int n = 3;
    const std::uint64_t moves = hanoi(n, 'A', 'C', 'B');
    std::cout << n << " disks: " << moves << " moves\n";
}
