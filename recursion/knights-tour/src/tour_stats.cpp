// tour_stats.cpp - how often Warnsdorff's rule alone finds a tour, and what
// backtracking costs, for every possible start on boards from 5 x 5 to 12 x 12.
// The output is deterministic and checked by the tests.
#include <cstdio>

#include "knights_tour.hpp"

int main()
{
    using knight::Board;
    using knight::Square;

    std::printf("Board  Starts  Warnsdorff alone  With backtracking  Most squares placed\n");
    for (int n = 5; n <= 12; ++n) {
        int starts = 0, greedy = 0, found = 0;
        long long most = 0;
        for (int r = 0; r < n; ++r)
            for (int c = 0; c < n; ++c) {
                if (n % 2 == 1 && (r + c) % 2 == 1)
                    continue;                       // no tour can start here
                ++starts;
                Board g(n);
                if (knight::warnsdorff(g, {r, c}, knight::Ties::last) == n * n)
                    ++greedy;
                Board b(n);
                const knight::Search s = knight::find_tour(b, {r, c}, true, 100'000'000);
                if (s.found && knight::is_tour(b))
                    ++found;
                if (s.placed > most)
                    most = s.placed;
            }
        std::printf("%2dx%-2d  %6d  %16d  %17d  %19lld\n", n, n, starts, greedy, found, most);
    }

    Board stuck(8);
    std::printf("\nWarnsdorff alone on 8x8 from (4, 2): stuck after %d of 64 squares\n",
                knight::warnsdorff(stuck, {4, 2}, knight::Ties::last));

    std::printf("\nBacktracking from the corner, squares placed before the first tour:\n");
    for (int n = 5; n <= 8; ++n) {
        Board plain(n), ordered(n);
        const knight::Search p = knight::find_tour(plain, {0, 0}, false, 1'000'000'000);
        const knight::Search w = knight::find_tour(ordered, {0, 0}, true, 1'000'000'000);
        std::printf("  %dx%d: %11lld in plain move order, %3lld in Warnsdorff order\n", n, n,
                    p.placed, w.placed);
    }
}
