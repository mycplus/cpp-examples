// main.cpp - the game loop: read keys, advance one tick, draw what changed,
// and keep ticks on a fixed schedule with std::chrono.
// Usage: brick [novice|advanced|expert]
#include "game.hpp"
#include "screen.hpp"
#include "terminal.hpp"

#include <chrono>
#include <cstdio>
#include <exception>
#include <iostream>
#include <random>
#include <string>
#include <thread>

namespace {

struct Level {
    const char* name;
    std::chrono::milliseconds tick;
};
constexpr Level levels[] = {{"novice", std::chrono::milliseconds(70)},
                            {"advanced", std::chrono::milliseconds(50)},
                            {"expert", std::chrono::milliseconds(35)}};

std::string status(const brick::Game& g, const std::string& msg)
{
    char line[96];
    std::snprintf(line, sizeof line, "Score %4d   Balls %d   Bricks %3d   %-26s\n",
                  g.score(), g.balls(), g.bricks_left(), msg.c_str());
    return line;
}

int play(const Level& level)
{
    using clock = std::chrono::steady_clock;
    brick::Terminal term;                    // restored when play() returns or throws
    brick::Game g(std::random_device{}());
    std::string prev, msg = "Arrows or A/D, P, Q";
    bool paused = false, quit = false;
    auto next = clock::now();

    while (!quit) {
        auto in = brick::Input::none;
        for (brick::Key k; !quit && (k = term.key()) != brick::Key::none;) {
            if (k == brick::Key::quit) quit = true;
            else if (k == brick::Key::pause) paused = !paused;
            else in = (k == brick::Key::left) ? brick::Input::left : brick::Input::right;
        }
        if (!paused && !quit) {
            const unsigned ev = g.step(in);
            if (ev & brick::event::win)            msg = "You win!";
            else if (ev & brick::event::game_over) msg = "Game over";
            else if (ev & brick::event::ball_lost) msg = g.balls() == 1 ? "Last ball!" : "Ball lost";
            else if (ev & brick::event::brick)     msg.clear();
        }
        std::string cur = g.render() + status(g, paused ? "Paused" : msg);
        term.write(prev.empty() ? brick::full_redraw(cur) : brick::diff_redraw(prev, cur));
        prev = std::move(cur);
        if (g.over())
            break;

        // Sleep until the next tick's start time, not for a fixed delay.
        next += level.tick;
        const auto now = clock::now();
        if (next > now)
            std::this_thread::sleep_until(next);
        else if (now - next > std::chrono::milliseconds(250))
            next = now;                      // after a stall, do not race to catch up
    }
    return g.score();
}

}  // namespace

int main(int argc, char** argv)
{
    const Level* level = &levels[0];
    if (argc > 1) {
        level = nullptr;
        for (const Level& l : levels)
            if (std::string(argv[1]) == l.name)
                level = &l;
        if (level == nullptr) {
            std::cerr << "usage: " << argv[0] << " [novice|advanced|expert]\n";
            return 1;
        }
    }
    try {
        const int score = play(*level);
        std::cout << "Final score: " << score << '\n';
    } catch (const std::exception& e) {
        std::cerr << argv[0] << ": " << e.what() << '\n';
        return 1;
    }
}
