// Tests for knights_tour.hpp.
#include "knights_tour.hpp"

#include <cstdlib>
#include <iostream>

namespace {

int failures = 0;
void check(bool ok, const char* expr, int line)
{
    if (!ok) {
        std::cerr << __FILE__ << ':' << line << ": CHECK failed: " << expr << '\n';
        ++failures;
    }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

using knight::Board;

Board from_rows(const int (&rows)[8][8])
{
    Board b(8);
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            b.set({r, c}, rows[r][c]);
    return b;
}

// The board the original MYCPLUS program printed for a start at (1, 1).
constexpr int original_sample[8][8] = {
    {55, 42, 15, 12, 57, 32, 17, 10}, {14, 1, 56, 43, 16, 11, 34, 31},
    {41, 54, 13, 64, 33, 58, 9, 18},  {2, 25, 46, 51, 44, 63, 30, 35},
    {47, 40, 53, 26, 59, 36, 19, 8},  {24, 3, 50, 45, 52, 27, 62, 29},
    {39, 48, 5, 22, 37, 60, 7, 20},   {4, 23, 38, 49, 6, 21, 28, 61}};

// What the original program printed for a start at (4, 2): square (0, 4)
// is never visited and 59 is missing, but no error was reported.
constexpr int original_broken[8][8] = {
    {45, 42, 13, 60, 0, 32, 11, 34},  {14, 55, 46, 43, 12, 35, 62, 31},
    {41, 44, 57, 54, 61, 64, 33, 10}, {56, 15, 52, 47, 36, 25, 30, 63},
    {51, 40, 1, 58, 53, 48, 9, 26},   {16, 19, 50, 37, 24, 27, 6, 29},
    {39, 2, 21, 18, 49, 4, 23, 8},    {20, 17, 38, 3, 22, 7, 28, 5}};

void test_checker()
{
    CHECK(knight::is_tour(from_rows(original_sample)));
    CHECK(knight::is_closed(from_rows(original_sample)));   // 64 is a knight move from 1
    CHECK(!knight::is_tour(from_rows(original_broken)));

    Board swapped = from_rows(original_sample);   // two squares exchanged
    swapped.set({0, 0}, 42);
    swapped.set({0, 1}, 55);
    CHECK(!knight::is_tour(swapped));
}

void test_greedy_matches_original()
{
    // Pure Warnsdorff with ties going to the last candidate is the original
    // program's rule: it reproduces its sample board exactly...
    Board b(8);
    CHECK(knight::warnsdorff(b, {1, 1}, knight::Ties::last) == 64);
    bool same = true;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            same = same && b.at({r, c}) == original_sample[r][c];
    CHECK(same);

    // ...and gets stuck where the original went wrong.
    Board stuck(8);
    CHECK(knight::warnsdorff(stuck, {4, 2}, knight::Ties::last) == 60);
}

void test_every_start_small_boards()
{
    for (int n = 5; n <= 10; ++n)
        for (int r = 0; r < n; ++r)
            for (int c = 0; c < n; ++c) {
                const bool possible = n % 2 == 0 || (r + c) % 2 == 0;
                if (!possible && n > 5)
                    continue;      // proving "no tour" by search is only quick on 5 x 5
                Board b(n);
                const knight::Search s = knight::find_tour(b, {r, c}, true, 100'000'000);
                if (s.found != possible || (s.found && !knight::is_tour(b)) || s.gave_up) {
                    CHECK(!"wrong result for a start square");
                    std::cerr << "  n=" << n << " start=(" << r << ", " << c << ")\n";
                    return;
                }
            }
}

void test_no_tour_boards()
{
    for (int n = 2; n <= 4; ++n) {
        Board b(n);
        const knight::Search s = knight::find_tour(b, {0, 0}, false, 100'000'000);
        CHECK(!s.found && !s.gave_up);             // exhaustive search, no tour
    }
    Board one(1);
    CHECK(knight::find_tour(one, {0, 0}, true, 10).found);
}

void test_limit()
{
    Board b(8);
    const knight::Search s = knight::find_tour(b, {0, 0}, false, 1000);
    CHECK(!s.found && s.gave_up && s.placed == 1000);
}

}  // namespace

int main()
{
    test_checker();
    test_greedy_matches_original();
    test_every_start_small_boards();
    test_no_tour_boards();
    test_limit();
    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "knights_tour: all tests passed\n";
    return EXIT_SUCCESS;
}
