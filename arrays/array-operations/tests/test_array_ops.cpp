#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "array_ops.hpp"

namespace {

int failures = 0;

void check(bool ok, const std::string &what)
{
    std::cout << "  " << what;
    for (std::size_t i = what.size(); i < std::size_t{56}; ++i) std::cout << ' ';
    std::cout << (ok ? " ok" : " FAIL") << '\n';
    if (!ok) ++failures;
}

} // namespace

int main()
{
    using mycplus::find_index;

    std::cout << "find_index on std::array\n";
    {
        const std::array<int, 3> a = { 10, 20, 30 };
        check(find_index(a, 10) == std::optional<std::size_t>{0}, "finds the first element");
        check(find_index(a, 30) == std::optional<std::size_t>{2}, "finds the last element");
        check(!find_index(a, 99),                                 "reports a missing value");
    }

    std::cout << "\nfind_index on std::vector\n";
    {
        const std::vector<int> v = { 5, 6, 7, 6 };
        check(find_index(v, 6) == std::optional<std::size_t>{1}, "returns the first match, not the last");
        check(!find_index(v, 0),                                 "reports a missing value");
        check(!find_index(std::vector<int>{}, 1),                "empty container finds nothing");
    }

    std::cout << "\nvector insertion and deletion\n";
    {
        std::vector<int> v = { 1, 2, 3 };
        v.insert(v.begin(), 0);
        check(v.size() == 4 && v.front() == 0 && v.back() == 3, "insert at begin shifts the rest right");
        v.insert(v.end(), 9);
        check(v.size() == 5 && v.back() == 9,                   "insert at end appends");
        v.erase(v.begin());
        check(v.size() == 4 && v.front() == 1,                  "erase at begin shifts left");
        v.erase(v.end() - 1);
        check(v.size() == 3 && v.back() == 3,                   "erase at end removes the last element");
    }
    {
        std::vector<int> v = { 1, 2, 3, 4 };
        const std::vector<int> before = v;
        const auto it = v.insert(v.begin() + 2, 99);
        v.erase(it);
        check(v == before, "insert then erase at the same position restores the vector");
    }

    std::cout << "\nsize and bounds\n";
    {
        std::array<int, 4> a{};
        check(a.size() == 4,                       "std::array::size() is the element count");
        check(sizeof a == 4 * sizeof(int),         "std::array adds no storage overhead");
        std::vector<int> v;
        v.reserve(10);
        v.push_back(1);
        check(v.size() == 1 && v.capacity() >= 10, "size and capacity are different things");
        bool threw = false;
        try { (void)v.at(5); } catch (const std::out_of_range &) { threw = true; }
        check(threw, "at() throws std::out_of_range for a bad index");
    }

    std::cout << '\n' << (failures ? "FAILURES" : "all checks passed") << '\n';
    return failures != 0;
}
