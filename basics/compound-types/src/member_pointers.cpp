// member_pointers.cpp - a pointer to member names a member of a class, not a
// member of one object. Applied to an object with .* or ->*, it selects that
// member. Here one sort function sorts by whichever field it is given.
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct Employee {
    std::string name;
    int age;
    double salary;

    double monthly() const { return salary / 12; }
};

template <typename T>
void sort_by(std::vector<Employee>& staff, T Employee::*field)
{
    std::sort(staff.begin(), staff.end(),
              [field](const Employee& a, const Employee& b) { return a.*field < b.*field; });
}

void print(const char* title, const std::vector<Employee>& staff)
{
    std::cout << title << ':';
    for (const Employee& e : staff)
        std::cout << ' ' << e.name;
    std::cout << '\n';
}

int main()
{
    std::vector<Employee> staff{
        {"Maya", 41, 98000}, {"Omar", 29, 87000}, {"Lena", 35, 121000}, {"Ravi", 52, 76000}};

    sort_by(staff, &Employee::name);
    print("by name  ", staff);
    sort_by(staff, &Employee::age);
    print("by age   ", staff);
    sort_by(staff, &Employee::salary);
    print("by salary", staff);

    int Employee::*field = &Employee::age;          // pointer to data member
    double (Employee::*pay)() const = &Employee::monthly;   // pointer to member function

    const Employee& first = staff.front();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << first.name << ": age " << first.*field << ", monthly " << (first.*pay)()
              << ", via std::invoke " << std::invoke(pay, first) << '\n';
}
