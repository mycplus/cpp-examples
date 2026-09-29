// autoplay.cpp - plays one game with the autopilot, without a terminal, and
// reports the result and what drawing each tick would cost.
// Usage: autoplay [seed]
#include "autopilot.hpp"
#include "game.hpp"
#include "screen.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>

int main(int argc, char** argv)
{
    constexpr long snapshot_tick = 300;
    const auto seed = static_cast<std::uint32_t>(argc > 1 ? std::strtoul(argv[1], nullptr, 10) : 1);
    brick::Game g(seed);
    std::string prev = g.render();
    unsigned long long full_bytes = 0, diff_bytes = 0;
    long ticks = 0;
    int paddle_hits = 0, balls_lost = 0;
    while (!g.over() && ticks < 100000) {
        const unsigned ev = g.step(brick::autopilot(g));
        ++ticks;
        paddle_hits += (ev & brick::event::paddle) != 0;
        balls_lost += (ev & brick::event::ball_lost) != 0;
        std::string cur = g.render();
        full_bytes += brick::full_redraw(cur).size();
        diff_bytes += brick::diff_redraw(prev, cur).size();
        if (ticks == snapshot_tick)
            std::printf("After %ld ticks (score %d):\n%s", snapshot_tick, g.score(), cur.c_str());
        prev = std::move(cur);
    }
    std::printf("\nseed %lu: %s after %ld ticks, score %d, %d paddle hits, %d balls lost\n",
                static_cast<unsigned long>(seed),
                g.bricks_left() == 0 ? "cleared every brick" : "game over", ticks, g.score(),
                paddle_hits, balls_lost);
    std::printf("output per tick: %.1f bytes redrawing the whole screen, "
                "%.1f bytes redrawing changed cells\n",
                static_cast<double>(full_bytes) / static_cast<double>(ticks),
                static_cast<double>(diff_bytes) / static_cast<double>(ticks));
}
