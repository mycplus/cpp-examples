// lcs.cpp - the longest common subsequence in C++17, for strings and for
// any sequences whose elements compare with ==, plus a line diff built on it.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic lcs.cpp -o lcs
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// The table of LCS lengths: cell (i, j) holds the LCS length of the first
// i elements of a and the first j elements of b. Works for std::string,
// std::string_view, std::vector and any other random-access sequence.
template <class A, class B>
std::vector<std::uint32_t> lcs_table(const A& a, const B& b)
{
    const std::size_t n = a.size(), m = b.size();
    if (n >= UINT32_MAX || m >= UINT32_MAX || (m + 1) > SIZE_MAX / (n + 1))
        throw std::length_error("inputs too long for an LCS table");
    std::vector<std::uint32_t> L((n + 1) * (m + 1), 0);
    auto at = [&](std::size_t i, std::size_t j) -> std::uint32_t& { return L[i * (m + 1) + j]; };
    for (std::size_t i = 1; i <= n; ++i)
        for (std::size_t j = 1; j <= m; ++j)
            at(i, j) = a[i - 1] == b[j - 1] ? at(i - 1, j - 1) + 1
                                            : std::max(at(i - 1, j), at(i, j - 1));
    return L;
}

// The positions (i in a, j in b) of one LCS, in order.
template <class A, class B>
std::vector<std::pair<std::size_t, std::size_t>> lcs_pairs(const A& a, const B& b)
{
    const std::vector<std::uint32_t> L = lcs_table(a, b);
    const std::size_t m = b.size();
    auto at = [&](std::size_t i, std::size_t j) { return L[i * (m + 1) + j]; };
    std::vector<std::pair<std::size_t, std::size_t>> pairs(at(a.size(), m));
    std::size_t i = a.size(), j = m, k = pairs.size();
    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) {
            pairs[--k] = {i - 1, j - 1};
            --i;
            --j;
        } else if (at(i - 1, j) >= at(i, j - 1)) {
            --i;
        } else {
            --j;
        }
    }
    return pairs;
}

// One LCS of two strings.
std::string lcs(std::string_view a, std::string_view b)
{
    std::string result;
    for (auto [i, j] : lcs_pairs(a, b)) {
        (void)j;
        result += a[i];
    }
    return result;
}

// The LCS length in O(min(n, m)) memory: two rows of the table.
std::size_t lcs_length(std::string_view a, std::string_view b)
{
    if (b.size() > a.size())
        std::swap(a, b);
    std::vector<std::size_t> prev(b.size() + 1, 0), cur(b.size() + 1, 0);
    for (std::size_t i = 1; i <= a.size(); ++i) {
        for (std::size_t j = 1; j <= b.size(); ++j)
            cur[j] = a[i - 1] == b[j - 1] ? prev[j - 1] + 1 : std::max(prev[j], cur[j - 1]);
        std::swap(prev, cur);
    }
    return prev[b.size()];
}

// A line diff: ' ' for a line in both, '-' only in a, '+' only in b.
struct DiffLine {
    char op;
    std::string text;
};

std::vector<DiffLine> diff(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    std::vector<DiffLine> out;
    std::size_t i = 0, j = 0;
    for (auto [ai, bj] : lcs_pairs(a, b)) {
        for (; i < ai; ++i)
            out.push_back({'-', a[i]});
        for (; j < bj; ++j)
            out.push_back({'+', b[j]});
        out.push_back({' ', a[i]});
        ++i;
        ++j;
    }
    for (; i < a.size(); ++i)
        out.push_back({'-', a[i]});
    for (; j < b.size(); ++j)
        out.push_back({'+', b[j]});
    return out;
}

int main()
{
    const std::pair<std::string_view, std::string_view> pairs[] = {
        {"ABCBDAB", "BDCABA"}, {"AGGTAB", "GXTXAYB"}, {"dynamic", "programming"}, {"abc", "xyz"}};
    for (auto [a, b] : pairs)
        std::cout << a << " / " << b << ": length " << lcs_length(a, b) << ", \"" << lcs(a, b)
                  << "\"\n";

    // The same template works on any element type: here, two lists of ints.
    const std::vector<int> v1 = {3, 1, 4, 1, 5, 9, 2, 6}, v2 = {1, 4, 5, 9, 2, 6, 5, 3};
    std::cout << "common subsequence of two int lists:";
    for (auto [i, j] : lcs_pairs(v1, v2)) {
        (void)j;
        std::cout << ' ' << v1[i];
    }
    std::cout << '\n';

    const std::vector<std::string> before = {"int total = 0;", "for (int x : v)", "    total += x;",
                                             "return total;"};
    const std::vector<std::string> after = {"long long total = 0;", "for (int x : v)",
                                            "    total += x;", "std::cout << total;",
                                            "return total;"};
    std::cout << "diff:\n";
    for (const DiffLine& d : diff(before, after))
        std::cout << d.op << ' ' << d.text << '\n';
    return 0;
}
