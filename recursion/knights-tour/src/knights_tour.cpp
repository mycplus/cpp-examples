// knights_tour.cpp - finds a knight's tour and prints the board.
// Usage: knights_tour [N [ROW COL]]   N is 1 to 30, ROW and COL are 0 to N-1.
// Defaults: an 8 x 8 board, starting in the corner (0, 0).
#include <charconv>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <system_error>

#include "knights_tour.hpp"

namespace {

constexpr int max_size = 30;

bool parse(std::string_view text, int low, int high, int& out)
{
    int v = 0;
    const auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), v);
    if (ec != std::errc{} || end != text.data() + text.size() || v < low || v > high)
        return false;
    out = v;
    return true;
}

}  // namespace

int main(int argc, char** argv)
{
    int n = 8;
    knight::Square start{0, 0};
    const bool ok = (argc == 1) ||
                    (argc == 2 && parse(argv[1], 1, max_size, n)) ||
                    (argc == 4 && parse(argv[1], 1, max_size, n) &&
                     parse(argv[2], 0, n - 1, start.row) && parse(argv[3], 0, n - 1, start.col));
    if (!ok) {
        std::cerr << "usage: knights_tour [N [ROW COL]]   (N is 1 to " << max_size
                  << ", ROW and COL are 0 to N-1)\n";
        return 2;
    }

    std::cout << "Knight's tour, " << n << " x " << n << " board, starting at (" << start.row
              << ", " << start.col << "):\n";

    // On an odd board a tour alternates colors and has one more square of
    // the corner color, so it must start on a square of that color.
    if (n % 2 == 1 && (start.row + start.col) % 2 == 1) {
        std::cout << "No tour starts here: on an odd board it must start on a square "
                     "the color of the corners.\n";
        return 1;
    }

    knight::Board board(n);
    const knight::Search s = knight::find_tour(board, start, true, 100'000'000);
    if (!s.found) {
        std::cout << (s.gave_up ? "Gave up" : "No tour exists") << " (searched "
                  << s.placed << " placements).\n";
        return 1;
    }

    const int width = n * n >= 100 ? 4 : 3;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c)
            std::cout << std::setw(width) << board.at({r, c});
        std::cout << '\n';
    }
    std::cout << (knight::is_closed(board) ? "Closed" : "Open") << " tour: " << n * n - 1
              << " moves, " << s.placed - n * n << " backtracked.\n";
    return knight::is_tour(board) ? 0 : 3;
}
