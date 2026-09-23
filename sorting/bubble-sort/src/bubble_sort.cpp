// bubble_sort.cpp - generic bubble sort in C++17
#include <algorithm>
#include <forward_list>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

// Sorts [first, last) so that comp(b, a) is false for every adjacent pair a, b.
// Needs only forward iterators. Stable: equal elements are never swapped.
template <class ForwardIt, class Compare = std::less<>>
void bubble_sort(ForwardIt first, ForwardIt last, Compare comp = {})
{
    if (first == last)
        return;

    ForwardIt end = last;                 // [end, last) is in final position
    for (;;) {
        ForwardIt last_swap = first;
        ForwardIt prev = first;
        for (ForwardIt cur = std::next(first); cur != end; ++prev, ++cur) {
            if (comp(*cur, *prev)) {
                std::iter_swap(prev, cur);
                last_swap = cur;
            }
        }
        if (last_swap == first)           // a full pass with no swaps
            return;
        end = last_swap;
    }
}

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
    std::vector<int> v{7, 3, 9, 2, 11, 15};
    bubble_sort(v.begin(), v.end());
    print("ascending: ", v);

    bubble_sort(v.begin(), v.end(), std::greater<>{});
    print("descending:", v);

    // A singly linked list: std::sort cannot take it, bubble_sort can.
    std::forward_list<std::string> words{"pear", "fig", "apple", "kiwi"};
    bubble_sort(words.begin(), words.end());
    print("words:     ", words);

    // Stability: sort by department only; names within a department keep order.
    std::vector<Employee> staff{
        {"Ava", 2}, {"Ben", 1}, {"Cleo", 2}, {"Dev", 1}, {"Eli", 3}, {"Fay", 1}};
    auto expected = staff;
    auto by_dept = [](const Employee& a, const Employee& b) { return a.dept < b.dept; };

    bubble_sort(staff.begin(), staff.end(), by_dept);
    std::stable_sort(expected.begin(), expected.end(), by_dept);

    std::cout << "by dept:   ";
    for (const auto& e : staff) std::cout << ' ' << e.dept << ':' << e.name;
    std::cout << '\n';

    bool same = std::equal(staff.begin(), staff.end(), expected.begin(),
                           [](const Employee& a, const Employee& b) {
                               return a.name == b.name && a.dept == b.dept;
                           });
    std::cout << "matches std::stable_sort: " << (same ? "yes" : "no") << '\n';
    return 0;
}
