// insertion_demo.cpp - the generic insertion sort on four containers
#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "insertion_sort.hpp"

template <class Range>
void print(const char* label, const Range& r)
{
    std::cout << label;
    for (const auto& x : r) std::cout << ' ' << x;
    std::cout << '\n';
}

struct Employee {
    std::string name;
    int dept;
};

int main()
{
    std::vector<int> v{29, 10, 14, 37, 13, 5, 41, 22};
    insertion_sort(v.begin(), v.end());
    print("ascending: ", v);

    insertion_sort(v.begin(), v.end(), std::greater<>{});
    print("descending:", v);

    std::list<std::string> words{"pear", "fig", "apple", "kiwi", "date"};
    insertion_sort(words.begin(), words.end());
    print("std::list: ", words);

    std::vector<int> w{29, 10, 14, 37, 13, 5, 41, 22};
    binary_insertion_sort(w.begin(), w.end());
    print("binary:    ", w);

    std::vector<Employee> staff{
        {"Ava", 2}, {"Ben", 1}, {"Cleo", 2}, {"Dev", 1}, {"Eli", 3}, {"Fay", 1}};
    auto expected = staff;
    auto by_dept = [](const Employee& a, const Employee& b) { return a.dept < b.dept; };
    insertion_sort(staff.begin(), staff.end(), by_dept);
    std::stable_sort(expected.begin(), expected.end(), by_dept);
    std::cout << "by dept:   ";
    for (const auto& e : staff) std::cout << ' ' << e.dept << ':' << e.name;
    bool same = std::equal(staff.begin(), staff.end(), expected.begin(),
                           [](const Employee& a, const Employee& b) { return a.name == b.name; });
    std::cout << "\nmatches std::stable_sort: " << (same ? "yes" : "no") << '\n';
    return 0;
}
