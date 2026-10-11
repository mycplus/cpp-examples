// identity.cpp - equality, identity and what assignment does in C++.
// A variable of class type holds the object itself, so assignment copies it.
#include <iostream>

struct Point {
    int x = 0, y = 0;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

int main() {
    Point p1{1, 2};
    Point p2{1, 2};
    std::cout << std::boolalpha;
    std::cout << "p1 == p2 (equal values):  " << (p1 == p2) << '\n';
    std::cout << "&p1 == &p2 (same object): " << (&p1 == &p2) << '\n';

    Point q = p1;          // copies the object
    q.x = 99;
    std::cout << "after q = p1; q.x = 99:   p1.x = " << p1.x << ", q.x = " << q.x << '\n';

    Point& r = p1;         // a reference is a second name for p1
    r.x = 99;
    std::cout << "after r = ref to p1; r.x = 99:   p1.x = " << p1.x << '\n';
}
