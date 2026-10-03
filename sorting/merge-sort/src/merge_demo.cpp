// merge_demo.cpp - the generic merge sort, and the standard library's
// merge-based sorts
#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "merge_sort.hpp"

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
    merge_sort(v.begin(), v.end());
    print("ascending: ", v);

    merge_sort(v.begin(), v.end(), std::greater<>{});
    print("descending:", v);

    std::vector<std::string> words{"pear", "fig", "apple", "kiwi", "date"};
    merge_sort(words.begin(), words.end());
    print("strings:   ", words);

    std::list<int> l{29, 10, 14, 37, 13, 5, 41, 22};
    l.sort();                                    // the list's own merge sort
    print("list.sort: ", l);

    std::vector<Employee> staff{
        {"Ava", 2}, {"Ben", 2}, {"Cleo", 1}, {"Dev", 3}, {"Eli", 1}, {"Fay", 2}};
    auto expected = staff;
    auto by_dept = [](const Employee& a, const Employee& b) { return a.dept < b.dept; };
    merge_sort(staff.begin(), staff.end(), by_dept);
    std::stable_sort(expected.begin(), expected.end(), by_dept);
    std::cout << "by dept:   ";
    for (const auto& e : staff) std::cout << ' ' << e.dept << ':' << e.name;
    bool same = std::equal(staff.begin(), staff.end(), expected.begin(),
                           [](const Employee& a, const Employee& b) { return a.name == b.name; });
    std::cout << "\nmatches std::stable_sort: " << (same ? "yes" : "no") << '\n';
    return 0;
}
