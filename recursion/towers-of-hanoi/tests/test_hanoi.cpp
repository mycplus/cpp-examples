// test_hanoi.cpp - checks the solvers by simulating the pegs.
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <vector>

#include "hanoi.hpp"

namespace {
int failures = 0;

void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::fprintf(stderr, "CHECK failed at line %d: %s\n", line, what);
        ++failures;
    }
}
#define CHECK(cond) check(static_cast<bool>(cond), #cond, __LINE__)

// Three pegs, each a stack of disk numbers; rejects any illegal move.
class Sim {
public:
    explicit Sim(int n) : n_(n)
    {
        for (int d = n; d >= 1; --d)
            pegs_[0].push_back(d);
    }
    void apply(const hanoi::Move& m)
    {
        ++moves_;
        if (illegal_)
            return;
        const int f = m.from - 'A', t = m.to - 'A';
        if (f < 0 || f > 2 || t < 0 || t > 2 || f == t || pegs_[f].empty() ||
            pegs_[f].back() != m.disk ||
            (!pegs_[t].empty() && pegs_[t].back() < m.disk)) {
            illegal_ = true;
            return;
        }
        pegs_[t].push_back(pegs_[f].back());
        pegs_[f].pop_back();
    }
    bool solved() const
    {
        if (illegal_ || !pegs_[0].empty() || !pegs_[1].empty() ||
            pegs_[2].size() != static_cast<std::size_t>(n_))
            return false;
        for (int i = 0; i < n_; ++i)
            if (pegs_[2][static_cast<std::size_t>(i)] != n_ - i)
                return false;
        return true;
    }
    std::uint64_t moves() const { return moves_; }

private:
    int n_;
    std::array<std::vector<int>, 3> pegs_;
    std::uint64_t moves_ = 0;
    bool illegal_ = false;
};

// Independent reference for move k: descend the recursion, no bit tricks.
hanoi::Move reference_move(int n, std::uint64_t k, char s, char t, char v)
{
    for (;;) {
        const std::uint64_t mid = std::uint64_t{1} << (n - 1);
        if (k == mid)
            return hanoi::Move{n, s, t};
        if (k < mid) {
            const char old_t = t;
            t = v;
            v = old_t;
        } else {
            k -= mid;
            const char old_s = s;
            s = v;
            v = old_s;
        }
        --n;
    }
}

std::uint64_t state = 0x9E3779B97F4A7C15ULL;
std::uint64_t next_rand()
{
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

template <typename F>
bool throws(F&& f)
{
    try {
        f();
    } catch (const std::exception&) {
        return true;
    }
    return false;
}
}  // namespace

int main()
{
    for (int n = 0; n <= 20; ++n) {
        const std::uint64_t expected = *hanoi::move_count(n);

        Sim rec(n);
        hanoi::solve(n, 'A', 'C', 'B', [&rec](const hanoi::Move& m) { rec.apply(m); });
        CHECK(rec.solved());
        CHECK(rec.moves() == expected);

        Sim it(n);
        hanoi::solve_iterative(n, 'A', 'C', 'B', [&it](const hanoi::Move& m) { it.apply(m); });
        CHECK(it.solved());
        CHECK(it.moves() == expected);

        const std::vector<hanoi::Move> a = hanoi::moves(n);
        std::vector<hanoi::Move> b;
        hanoi::solve_iterative(n, 'A', 'C', 'B', [&b](const hanoi::Move& m) { b.push_back(m); });
        CHECK(a == b);

        bool all_match = true;
        for (std::size_t i = 0; i < a.size(); ++i)
            all_match = all_match && hanoi::move_at(n, i + 1, 'A', 'C', 'B') == a[i];
        CHECK(all_match);
    }

    CHECK(*hanoi::move_count(63) == 9223372036854775807ULL);
    CHECK(*hanoi::move_count(64) == UINT64_MAX);
    CHECK(!hanoi::move_count(65));
    CHECK(!hanoi::move_count(-1));
    CHECK(throws([] { hanoi::solve(-1, 'A', 'C', 'B', [](const hanoi::Move&) {}); }));
    CHECK(throws([] { hanoi::solve_iterative(65, 'A', 'C', 'B', [](const hanoi::Move&) {}); }));
    CHECK(throws([] { hanoi::move_at(3, 0, 'A', 'C', 'B'); }));
    CHECK(throws([] { hanoi::move_at(3, 8, 'A', 'C', 'B'); }));
    CHECK(throws([] { hanoi::move_at(0, 1, 'A', 'C', 'B'); }));
    CHECK(throws([] { (void)hanoi::moves(31); }));

    // move_at for 64 disks against the reference, including the 64 moves
    // where k | (k - 1) == 2^64 - 1 and the last move.
    std::vector<std::uint64_t> ks{1, UINT64_MAX};
    for (int j = 0; j < 64; ++j)
        ks.push_back(UINT64_MAX - ((std::uint64_t{1} << j) - 1));
    while (ks.size() < 2000) {
        const std::uint64_t k = next_rand();
        ks.push_back(k == 0 ? 1 : k);
    }
    bool all_match = true;
    for (std::uint64_t k : ks)
        all_match = all_match &&
                    hanoi::move_at(64, k, 'A', 'C', 'B') == reference_move(64, k, 'A', 'C', 'B');
    CHECK(all_match);

    // Moving C to A via B.
    Sim relabelled(7);
    hanoi::solve(7, 'C', 'A', 'B', [&relabelled](const hanoi::Move& m) {
        relabelled.apply(hanoi::Move{m.disk, static_cast<char>('A' + 'C' - m.from),
                                     static_cast<char>('A' + 'C' - m.to)});
    });
    CHECK(relabelled.solved());

    if (failures != 0) {
        std::fprintf(stderr, "%d check(s) failed\n", failures);
        return EXIT_FAILURE;
    }
    std::puts("all tests passed");
    return EXIT_SUCCESS;
}
