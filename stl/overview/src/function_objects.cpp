// function_objects.cpp - customizing algorithms and containers with lambdas,
// standard function objects and a comparison for a user-defined type.
#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

struct Employee {
    std::string name;
    int salary;
};

int main()
{
    std::vector<int> v{5, 3, 8, 1, 9, 2};

    std::sort(v.begin(), v.end(), std::greater<int>{});        // descending
    std::cout << "descending:";
    for (int x : v) std::cout << ' ' << x;

    int threshold = 4;
    auto above = std::count_if(v.begin(), v.end(),
                               [threshold](int x) { return x > threshold; });
    std::cout << "\nabove " << threshold << ": " << above;

    std::vector<int> squares(v.size());
    std::transform(v.begin(), v.end(), squares.begin(), [](int x) { return x * x; });
    std::cout << "\nsum of squares: "
              << std::accumulate(squares.begin(), squares.end(), 0);

    // A set orders by its comparison object; here, by salary then name.
    auto by_salary = [](const Employee& a, const Employee& b) {
        return a.salary != b.salary ? a.salary < b.salary : a.name < b.name;
    };
    std::set<Employee, decltype(by_salary)> staff(by_salary);
    staff.insert({"dana", 5200});
    staff.insert({"erin", 4100});
    staff.insert({"finn", 5200});

    std::cout << "\nby salary:";
    for (const auto& e : staff) std::cout << ' ' << e.name << '(' << e.salary << ')';
    std::cout << '\n';
}
