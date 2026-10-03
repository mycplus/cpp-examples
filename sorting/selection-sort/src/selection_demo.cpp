// selection_demo.cpp - the generic selection sort on three containers,
// and what it does to equal keys
#include <algorithm>
#include <forward_list>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include "selection_sort.hpp"

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
    selection_sort(v.begin(), v.end());
    print("ascending:   ", v);

    selection_sort(v.begin(), v.end(), std::greater<>{});
    print("descending:  ", v);

    std::forward_list<std::string> words{"pear", "fig", "apple", "kiwi", "date"};
    selection_sort(words.begin(), words.end());
    print("forward_list:", words);

    std::vector<Employee> staff{
        {"Ava", 2}, {"Ben", 2}, {"Cleo", 1}, {"Dev", 3}};
    auto expected = staff;
    auto by_dept = [](const Employee& a, const Employee& b) { return a.dept < b.dept; };
    selection_sort(staff.begin(), staff.end(), by_dept);
    std::stable_sort(expected.begin(), expected.end(), by_dept);
    std::cout << "by dept:     ";
    for (const auto& e : staff) std::cout << ' ' << e.dept << ':' << e.name;
    bool same = std::equal(staff.begin(), staff.end(), expected.begin(),
                           [](const Employee& a, const Employee& b) { return a.name == b.name; });
    std::cout << "\nmatches std::stable_sort: " << (same ? "yes" : "no") << '\n';
    return 0;
}
