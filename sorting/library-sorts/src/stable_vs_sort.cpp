// stable_vs_sort.cpp - what std::sort does to equal keys, and what
// std::stable_sort does instead. 1,000 records, 10 departments.
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

struct Employee {
    std::string name;   // e0, e1, ... in input order
    int dept;
};

static void show(const char* label, const std::vector<Employee>& v)
{
    std::cout << label;
    int shown = 0;
    for (const auto& e : v)                 // the first five in department 0
        if (e.dept == 0 && shown++ < 5)
            std::cout << ' ' << e.name;
    std::cout << '\n';
}

int main()
{
    std::uint64_t xs = 88172645463325252ULL;
    std::vector<Employee> staff;
    for (int i = 0; i < 1000; ++i) {
        xs ^= xs << 13; xs ^= xs >> 7; xs ^= xs << 17;
        staff.push_back({"e" + std::to_string(i), static_cast<int>(xs % 10)});
    }
    auto by_dept = [](const Employee& a, const Employee& b) { return a.dept < b.dept; };

    auto a = staff, b = staff;
    std::sort(a.begin(), a.end(), by_dept);
    std::stable_sort(b.begin(), b.end(), by_dept);

    show("input order:      ", staff);
    show("std::sort:        ", a);
    show("std::stable_sort: ", b);
    return 0;
}
