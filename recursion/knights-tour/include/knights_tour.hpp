// knights_tour.hpp - knight's tours by Warnsdorff's rule, with and without
// backtracking, plus a checker. Header-only, C++17.
#ifndef KNIGHTS_TOUR_HPP
#define KNIGHTS_TOUR_HPP

#include <algorithm>
#include <array>
#include <cstdlib>
#include <vector>

namespace knight {

struct Square {
    int row, col;
};

// The eight knight moves, in the order the original MYCPLUS program used.
inline constexpr std::array<Square, 8> moves = {{
    {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}}};

// An n x n board. Each cell holds 0 if unvisited, else the move number 1..n*n.
class Board {
public:
    explicit Board(int n) : n_(n), cells_(static_cast<std::size_t>(n) * n, 0) {}

    int size() const noexcept { return n_; }
    bool inside(Square s) const noexcept
    {
        return s.row >= 0 && s.row < n_ && s.col >= 0 && s.col < n_;
    }
    int at(Square s) const { return cells_[index(s)]; }
    void set(Square s, int step) { cells_[index(s)] = step; }
    bool is_free(Square s) const { return inside(s) && at(s) == 0; }

    // Free squares a knight on s could move to next: Warnsdorff's measure.
    int exits(Square s) const
    {
        int count = 0;
        for (const Square m : moves)
            if (is_free({s.row + m.row, s.col + m.col}))
                ++count;
        return count;
    }

private:
    std::size_t index(Square s) const
    {
        return static_cast<std::size_t>(s.row) * static_cast<std::size_t>(n_) +
               static_cast<std::size_t>(s.col);
    }

    int n_;
    std::vector<int> cells_;
};

// Pure Warnsdorff: always move to the free square with the fewest exits,
// never undo a move. Ties go to the first or last such square in move order.
// Returns the number of squares visited; n*n means a complete tour.
enum class Ties { first, last };

inline int warnsdorff(Board& b, Square start, Ties ties)
{
    int step = 1;
    b.set(start, step);
    for (Square at = start; step < b.size() * b.size(); ++step) {
        Square best{};
        int best_exits = 9;
        for (const Square m : moves) {
            const Square next{at.row + m.row, at.col + m.col};
            if (!b.is_free(next))
                continue;
            const int e = b.exits(next);
            if (e < best_exits || (ties == Ties::last && e == best_exits)) {
                best = next;
                best_exits = e;
            }
        }
        if (best_exits == 9)
            break;                                  // no free square: stuck
        b.set(best, step + 1);
        at = best;
    }
    return step;
}

// Depth-first search for a tour. With `ordered`, moves are tried in
// Warnsdorff order and dead ends are skipped; without it, in plain move order.
// Gives up after `limit` squares have been placed.
struct Search {
    bool found = false;
    bool gave_up = false;
    long long placed = 0;                           // squares placed, retries included
};

namespace detail {
// Squared distance from the center, in half-squares, so it stays an integer.
// Ties in Warnsdorff's rule go to the square nearer the edge.
inline int center_distance(int n, Square s)
{
    const int dr = 2 * s.row - (n - 1), dc = 2 * s.col - (n - 1);
    return dr * dr + dc * dc;
}

inline bool extend(Board& b, Square at, int step, bool ordered, long long limit, Search& s)
{
    const int last = b.size() * b.size();
    if (step == last)
        return true;
    std::array<Square, 8> next{};
    std::array<int, 8> exits{}, distance{};
    int count = 0;
    for (const Square m : moves) {
        const Square sq{at.row + m.row, at.col + m.col};
        if (!b.is_free(sq))
            continue;
        const int e = ordered ? b.exits(sq) : 0;
        if (ordered && e == 0 && step + 1 != last)
            continue;                               // a dead end before the last square
        next[count] = sq;
        exits[count] = e;
        distance[count] = ordered ? center_distance(b.size(), sq) : 0;
        ++count;
    }
    std::array<int, 8> order{0, 1, 2, 3, 4, 5, 6, 7};
    std::stable_sort(order.begin(), order.begin() + count, [&](int a, int c) {
        if (exits[a] != exits[c])
            return exits[a] < exits[c];             // Warnsdorff: fewest exits first
        return distance[a] > distance[c];           // tie: nearer the edge first
    });
    for (int i = 0; i < count; ++i) {
        if (s.placed >= limit) {
            s.gave_up = true;
            return false;
        }
        const Square sq = next[order[i]];
        b.set(sq, step + 1);
        ++s.placed;
        if (extend(b, sq, step + 1, ordered, limit, s))
            return true;
        b.set(sq, 0);                               // undo and try the next move
        if (s.gave_up)
            return false;
    }
    return false;
}
}  // namespace detail

inline Search find_tour(Board& b, Square start, bool ordered, long long limit)
{
    Search s;
    b.set(start, 1);
    s.placed = 1;
    s.found = detail::extend(b, start, 1, ordered, limit, s);
    return s;
}

// True if the board holds 1..n*n exactly once, each step a knight move.
inline bool is_tour(const Board& b)
{
    const int n = b.size();
    std::vector<Square> where(static_cast<std::size_t>(n) * n + 1, Square{-1, -1});
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < n; ++c) {
            const int v = b.at({r, c});
            if (v < 1 || v > n * n || where[static_cast<std::size_t>(v)].row != -1)
                return false;
            where[static_cast<std::size_t>(v)] = {r, c};
        }
    for (std::size_t v = 1; v < where.size() - 1; ++v) {
        const int dr = std::abs(where[v].row - where[v + 1].row);
        const int dc = std::abs(where[v].col - where[v + 1].col);
        if (!((dr == 1 && dc == 2) || (dr == 2 && dc == 1)))
            return false;
    }
    return true;
}

// A tour is closed if the last square is a knight move from the first.
inline bool is_closed(const Board& b)
{
    const int n = b.size();
    Square first{-1, -1}, last{-1, -1};
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < n; ++c) {
            if (b.at({r, c}) == 1) first = {r, c};
            if (b.at({r, c}) == n * n) last = {r, c};
        }
    const int dr = std::abs(first.row - last.row), dc = std::abs(first.col - last.col);
    return (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
}

}  // namespace knight

#endif
