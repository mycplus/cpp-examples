// main.cpp - the game loop: read keys, advance one tick, draw what changed,
// and keep ticks on a deadline with std::chrono.
// Usage: race [easy|normal|hard]
#include "race.hpp"
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

std::string status(const race::Game& g, const char* msg)
{
    const long long tenths = g.time_left().count() / 100;
    std::string tank(10, '-');
    for (int i = 0; i < g.nitro() * 10 / race::nitro_ticks; ++i)
        tank[static_cast<std::size_t>(i)] = '#';
    char line[96];
    std::snprintf(line, sizeof line, "Dist %3d/%d  Time %2lld.%llds\nNitro [%s] %-3s  %-13s\n",
                  g.distance(), race::track_len, tenths / 10, tenths % 10, tank.c_str(),
                  g.boosting() ? "ON" : "", msg);
    return line;
}

int play(race::Level level)
{
    using clock = std::chrono::steady_clock;
    race::Terminal term;                      // restored when play() returns or throws
    race::Game g(std::random_device{}(), level);
    const char* msg = "Arrows/WAD, P, Q";
    std::string prev;
    bool paused = false, quit = false;
    auto next = clock::now();

    while (!quit) {
        auto in = race::Input::none;
        for (race::Key k; !quit && (k = term.key()) != race::Key::none;) {
            if (k == race::Key::quit) quit = true;
            else if (k == race::Key::pause) paused = !paused;
            else if (k == race::Key::nitro) in = race::Input::nitro;
            else in = (k == race::Key::left) ? race::Input::left : race::Input::right;
        }
        if (!paused && !quit) {
            const unsigned ev = g.step(in);
            if (ev & race::event::crash)        msg = "Crashed!";
            else if (ev & race::event::finish)  msg = "Finished!";
            else if (ev & race::event::time_up) msg = "Out of time";
        }
        std::string cur = g.render() + status(g, paused ? "Paused" : msg);
        term.write(prev.empty() ? race::full_redraw(cur) : race::diff_redraw(prev, cur));
        prev = std::move(cur);
        if (g.over())
            break;

        // Sleep until the next tick's start time; nitro makes ticks shorter.
        next += g.last_tick();
        const auto now = clock::now();
        if (next > now)
            std::this_thread::sleep_until(next);
        else if (now - next > std::chrono::milliseconds(250))
            next = now;                       // after a stall, do not race to catch up
    }
    return g.distance();
}

}  // namespace

int main(int argc, char** argv)
{
    auto level = race::Level::normal;
    if (argc > 1) {
        const std::string arg = argv[1];
        if (arg == "easy") level = race::Level::easy;
        else if (arg == "hard") level = race::Level::hard;
        else if (arg != "normal") {
            std::cerr << "usage: " << argv[0] << " [easy|normal|hard]\n";
            return 1;
        }
    }
    try {
        const int distance = play(level);
        std::cout << "Distance: " << distance << " of " << race::track_len << '\n';
    } catch (const std::exception& e) {
        std::cerr << argv[0] << ": " << e.what() << '\n';
        return 1;
    }
}
