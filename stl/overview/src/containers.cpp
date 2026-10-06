// containers.cpp - one short example from each container family.
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

int main()
{
    // Sequence containers keep elements in the order you put them.
    std::vector<int> v{3, 1, 4};
    v.push_back(1);                              // amortized O(1) at the end
    std::deque<int> d{3, 1, 4};
    d.push_front(0);                             // O(1) at either end
    std::list<int> l{3, 1, 4};
    l.insert(std::next(l.begin()), 9);           // O(1) anywhere, given an iterator
    std::array<int, 3> a{3, 1, 4};               // fixed size, no heap allocation

    std::cout << "vector:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << "\ndeque: ";
    for (int x : d) std::cout << ' ' << x;
    std::cout << "\nlist:  ";
    for (int x : l) std::cout << ' ' << x;
    std::cout << "\narray: ";
    for (int x : a) std::cout << ' ' << x;

    // Ordered associative containers keep elements sorted by key.
    std::set<int> s{3, 1, 4, 1, 5};              // duplicates are dropped
    std::map<std::string, int> ages{{"carol", 41}, {"alice", 36}};
    ages["bob"] = 29;

    std::cout << "\nset:   ";
    for (int x : s) std::cout << ' ' << x;
    std::cout << "\nmap:   ";
    for (const auto& [name, age] : ages) std::cout << ' ' << name << '=' << age;

    // Unordered associative containers use hashing; iteration order is unspecified.
    std::unordered_map<std::string, int> stock{{"apples", 4}, {"pears", 0}};
    stock["plums"] = 7;
    std::cout << "\nunordered_map has " << stock.size() << " keys; plums = "
              << stock.at("plums") << '\n';
}
