// Tests for src/screen.cpp. A small terminal emulator applies the output to a
// character grid, which must then show the new frame.
#include "autopilot.hpp"
#include "game.hpp"
#include "screen.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

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

struct Emulator {
    std::array<std::string, 30> grid;
    std::size_t row = 0, col = 0;

    // Understands exactly what screen.cpp emits; false on anything else.
    bool apply(const std::string& s)
    {
        for (std::size_t i = 0; i < s.size();) {
            if (s.compare(i, 3, "\x1b[H") == 0) { row = col = 0; i += 3; continue; }
            if (s.compare(i, 4, "\x1b[2J") == 0) {
                for (auto& line : grid) line.assign(80, ' ');
                i += 4;
                continue;
            }
            if (s[i] == '\x1b') {                              // ESC [ row ; col H
                const std::size_t semi = s.find(';', i), h = s.find('H', i);
                if (s.compare(i, 2, "\x1b[") != 0 || semi == std::string::npos || h == std::string::npos)
                    return false;
                row = std::stoul(s.substr(i + 2, semi - i - 2)) - 1;
                col = std::stoul(s.substr(semi + 1, h - semi - 1)) - 1;
                i = h + 1;
                continue;
            }
            if (s[i] == '\r') col = 0;
            else if (s[i] == '\n') ++row;
            else if (row < grid.size() && col < 80) grid[row][col++] = s[i];
            else return false;
            ++i;
        }
        return true;
    }

    bool shows(const std::string& frame) const
    {
        std::size_t r = 0, start = 0;
        while (start < frame.size()) {
            const std::size_t end = frame.find('\n', start);
            const std::string line = frame.substr(start, end - start);
            if (r >= grid.size() || grid[r].compare(0, line.size(), line) != 0)
                return false;
            ++r;
            start = end + 1;
        }
        return true;
    }
};

void test_small_frames()
{
    CHECK(brick::full_redraw("abc\ndef\n") == "\x1b[H\x1b[2J" "abc\r\ndef\r\n");
    CHECK(brick::diff_redraw("abc\ndef\n", "abc\ndxf\n") == "\x1b[2;2Hx");
    CHECK(brick::diff_redraw("abc\ndef\n", "abc\ndef\n").empty());
    CHECK(brick::diff_redraw("abcdef\n", "xbcdyz\n") == "\x1b[1;1Hx\x1b[1;5Hyz");
}

void test_full_game()
{
    brick::Game g(2);
    std::string prev = g.render();
    Emulator term;
    CHECK(term.apply(brick::full_redraw(prev)) && term.shows(prev));
    long ticks = 0;
    while (!g.over() && ticks < 100000) {
        g.step(brick::autopilot(g));
        std::string cur = g.render();
        CHECK(term.apply(brick::diff_redraw(prev, cur)));
        if (!term.shows(cur)) {
            CHECK(!"screen differs from frame");
            return;
        }
        prev = std::move(cur);
        ++ticks;
    }
    CHECK(g.over() && ticks > 0);
}

}  // namespace

int main()
{
    test_small_frames();
    test_full_game();
    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "screen: all tests passed\n";
    return EXIT_SUCCESS;
}
