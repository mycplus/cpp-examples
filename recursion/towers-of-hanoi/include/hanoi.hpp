// hanoi.hpp - Towers of Hanoi: recursive and iterative solvers, direct
// computation of any single move, and the move count. Header-only, C++17.
#ifndef MYCPLUS_HANOI_HPP
#define MYCPLUS_HANOI_HPP

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

namespace hanoi {

// Largest disk count for which 2^n - 1 fits in std::uint64_t.
inline constexpr int max_disks = 64;

struct Move {
    int disk;     // 1 is the smallest
    char from;
    char to;
    friend bool operator==(const Move& a, const Move& b)
    {
        return a.disk == b.disk && a.from == b.from && a.to == b.to;
    }
};

// 2^n - 1, or std::nullopt when n is negative or the count does not fit.
inline std::optional<std::uint64_t> move_count(int n)
{
    if (n < 0 || n > max_disks)
        return std::nullopt;
    if (n == 64)                       // a 64-bit shift by 64 is undefined
        return UINT64_MAX;
    return (std::uint64_t{1} << n) - 1;
}

namespace detail {
template <typename Visit>
void solve(int n, char source, char target, char spare, Visit& visit)
{
    if (n == 0)
        return;
    solve(n - 1, source, spare, target, visit);
    visit(Move{n, source, target});
    solve(n - 1, spare, target, source, visit);
}

inline int trailing_zeros(std::uint64_t k)     // k != 0
{
    int z = 0;
    for (; (k & 1u) == 0; k >>= 1)
        ++z;
    return z;
}
}  // namespace detail

// Recursive solution: calls visit(Move) for each move. Depth is n + 1.
template <typename Visit>
void solve(int n, char source, char target, char spare, Visit&& visit)
{
    if (n < 0)
        throw std::invalid_argument("hanoi::solve: negative disk count");
    detail::solve(n, source, target, spare, visit);
}

// Move k (1-based) of the n-disk solution, computed from the bits of k.
inline Move move_at(int n, std::uint64_t k, char source, char target, char spare)
{
    const auto last = move_count(n);
    if (!last || k == 0 || k > *last)
        throw std::out_of_range("hanoi::move_at: n or k out of range");

    // The bit formula moves the tower from peg index 0 to index 2 when n is
    // odd and to index 1 when n is even.
    const char peg[3] = {source, n % 2 == 1 ? spare : target,
                         n % 2 == 1 ? target : spare};
    // (k | (k - 1)) + 1 wraps to 0 when n == 64; reduce modulo 3 first.
    return Move{detail::trailing_zeros(k) + 1,
                peg[(k & (k - 1)) % 3],
                peg[((k | (k - 1)) % 3 + 1) % 3]};
}

// Iterative solution: the same sequence as solve(), with no recursion.
template <typename Visit>
void solve_iterative(int n, char source, char target, char spare, Visit&& visit)
{
    const auto last = move_count(n);
    if (!last)
        throw std::invalid_argument("hanoi::solve_iterative: n out of range");
    for (std::uint64_t k = 1; *last != 0; ++k) {
        visit(move_at(n, k, source, target, spare));
        if (k == *last)                // not k <= last: last may be UINT64_MAX
            break;
    }
}

// All moves as a vector. Limited to 30 disks (about a billion moves).
inline std::vector<Move> moves(int n, char source = 'A', char target = 'C',
                               char spare = 'B')
{
    if (n > 30)
        throw std::length_error("hanoi::moves: more than 30 disks");
    std::vector<Move> out;
    if (n > 0)
        out.reserve(static_cast<std::size_t>(*move_count(n)));
    solve(n, source, target, spare, [&out](const Move& m) { out.push_back(m); });
    return out;
}

}  // namespace hanoi

#endif
