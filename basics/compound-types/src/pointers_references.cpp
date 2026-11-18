// pointers_references.cpp - a pointer is an object holding an address; it can
// be null and can be pointed somewhere else. A reference is another name for
// an existing object; it must be bound when created and never changes target.
#include <iostream>

void add_bonus(int* score)          // the caller passes an address, which may be null
{
    if (score != nullptr)
        *score += 10;
}

void add_bonus(int& score)          // the caller passes an object; no null check needed
{
    score += 10;
}

int main()
{
    int alice = 50;
    int bob = 70;

    int* p = &alice;
    *p += 1;                         // changes alice through the pointer
    p = &bob;                        // the pointer now points to bob
    *p += 1;
    std::cout << "after pointer:   alice = " << alice << ", bob = " << bob << '\n';

    int& r = alice;                  // r is another name for alice, for good
    r = bob;                         // copies bob's value into alice; r still names alice
    r += 1;
    std::cout << "after reference: alice = " << alice << ", bob = " << bob << '\n';

    add_bonus(&bob);
    add_bonus(nullptr);              // allowed, and the function must check
    add_bonus(alice);
    std::cout << "after bonuses:   alice = " << alice << ", bob = " << bob << '\n';

    const int* read_only = &alice;   // pointer to const: cannot change alice through it
    int* const fixed = &bob;         // const pointer: cannot point elsewhere
    *fixed += 5;                     // allowed: the int it points to is not const
    std::cout << "read_only -> " << *read_only;
    read_only = &bob;                // allowed: the pointer itself is not const
    std::cout << ", re-pointed -> " << *read_only << ", fixed -> " << *fixed << '\n';
}
