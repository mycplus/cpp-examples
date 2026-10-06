// invalidated_iterator.cpp - an iterator kept across push_back. When the
// vector grows, it moves its elements and the old iterator dangles.
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> scores{70, 85, 90};
    std::cout << "capacity before: " << scores.capacity() << '\n';

    auto best = scores.begin() + 2;       // points at 90
    scores.push_back(95);                 // exceeds capacity: storage is reallocated

    std::cout << "capacity after:  " << scores.capacity() << '\n';
    std::cout << "best score: " << *best << '\n';   // reads freed memory
}
