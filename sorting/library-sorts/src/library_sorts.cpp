// library_sorts.cpp - std::sort, std::stable_sort and qsort compared:
// comparisons on 10,000 elements, stability on equal keys, and timings.
// Build: g++ -std=c++20 -Wall -Wextra -O2 library_sorts.cpp -o library_sorts
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::uint64_t xs = 88172645463325252ULL;   // same generator and seed as the C code
std::uint64_t xorshift64()
{
    xs ^= xs << 13; xs ^= xs >> 7; xs ^= xs << 17;
    return xs;
}

unsigned long long comparisons = 0;

int cmp_int(const void* pa, const void* pb)
{
    int a = *static_cast<const int*>(pa), b = *static_cast<const int*>(pb);
    ++comparisons;
    return (a > b) - (a < b);
}

int cmp_int_plain(const void* pa, const void* pb)   // no counter, for timing
{
    int a = *static_cast<const int*>(pa), b = *static_cast<const int*>(pb);
    return (a > b) - (a < b);
}

struct Employee {
    std::string name;
    int dept;
};

}  // namespace

int main()
{
    // 1. Comparisons on 10,000 elements
    std::vector<int> random(10000);
    for (int& v : random) v = static_cast<int>(xorshift64() % 1000000);
    std::vector<int> sorted = random;
    std::sort(sorted.begin(), sorted.end());
    std::vector<int> reversed(sorted.rbegin(), sorted.rend());

    auto counting_less = [](int a, int b) { ++comparisons; return a < b; };

    std::cout << "comparisons, n = 10000        random    sorted  reversed\n";
    for (int which = 0; which < 3; ++which) {
        const char* names[] = {"std::sort", "std::stable_sort", "qsort"};
        std::printf("%-24s", names[which]);
        for (const auto* in : {&random, &sorted, &reversed}) {
            std::vector<int> v = *in;
            comparisons = 0;
            if (which == 0)      std::sort(v.begin(), v.end(), counting_less);
            else if (which == 1) std::stable_sort(v.begin(), v.end(), counting_less);
            else                 std::qsort(v.data(), v.size(), sizeof(int), cmp_int);
            if (v != sorted) { std::cout << "WRONG RESULT\n"; return 1; }
            std::printf("%10llu", comparisons);
        }
        std::printf("\n");
    }

    // 2. Stability: 1,000 employees in 10 departments, sorted by department
    std::vector<Employee> staff;
    for (int i = 0; i < 1000; ++i)
        staff.push_back({"e" + std::to_string(i), static_cast<int>(xorshift64() % 10)});
    auto by_dept = [](const Employee& a, const Employee& b) { return a.dept < b.dept; };

    auto keeps_order = [](const std::vector<Employee>& v) {
        for (std::size_t i = 1; i < v.size(); ++i)
            if (v[i].dept == v[i - 1].dept &&
                std::stoi(v[i].name.substr(1)) < std::stoi(v[i - 1].name.substr(1)))
                return false;
        return true;
    };
    auto a = staff, b = staff;
    std::sort(a.begin(), a.end(), by_dept);
    std::stable_sort(b.begin(), b.end(), by_dept);
    std::cout << "\nequal keys kept in input order (1,000 records, 10 keys):\n"
              << "  std::sort        " << (keeps_order(a) ? "yes" : "no") << '\n'
              << "  std::stable_sort " << (keeps_order(b) ? "yes" : "no") << '\n';

    // 3. Median of 5 timings on 1,000,000 random ints
    std::vector<int> big(1000000);
    for (int& v : big) v = static_cast<int>(xorshift64() % 1000000000);
    std::vector<int> ref = big;
    std::sort(ref.begin(), ref.end());

    std::cout << "\nmilliseconds, n = 1000000 random, median of 5\n";
    for (int which = 0; which < 4; ++which) {
        const char* names[] = {"std::sort", "std::ranges::sort", "std::stable_sort",
                               "qsort"};
        std::vector<double> t;
        for (int r = 0; r < 5; ++r) {
            std::vector<int> v = big;
            auto t0 = std::chrono::steady_clock::now();
            if (which == 0)      std::sort(v.begin(), v.end());
            else if (which == 1) std::ranges::sort(v);
            else if (which == 2) std::stable_sort(v.begin(), v.end());
            else                 std::qsort(v.data(), v.size(), sizeof(int), cmp_int_plain);
            auto t1 = std::chrono::steady_clock::now();
            if (v != ref) { std::cout << "WRONG RESULT\n"; return 1; }
            t.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
        }
        std::sort(t.begin(), t.end());
        std::printf("  %-18s %8.2f\n", names[which], t[2]);
    }
    return 0;
}
