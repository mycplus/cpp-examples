// autoplay.cpp - drives three races with the autopilot and prints the results
// and one frame. The output is deterministic and checked by the tests.
#include "autopilot.hpp"
#include "race.hpp"

#include <cstdio>

int main()
{
    const struct { const char* name; race::Level level; } levels[] = {
        {"easy", race::Level::easy}, {"normal", race::Level::normal}, {"hard", race::Level::hard}};

    for (const auto& lv : levels) {
        race::Game g(1, lv.level);
        const race::Autopilot pilot(g);
        unsigned ev = 0;
        int steers = 0;
        while (!g.over()) {
            const race::Input in = pilot.next(g);
            steers += in == race::Input::left || in == race::Input::right;
            ev = g.step(in);
            if (lv.level == race::Level::normal && g.distance() == 408)
                std::printf("Normal traffic, seed 1, after 408 ticks:\n%s", g.render().c_str());
        }
        const char* result = (ev & race::event::finish) ? "finished"
                           : (ev & race::event::crash)  ? "crashed" : "out of time";
        const long long used = (race::time_limit - g.time_left()).count();
        std::printf("%-6s seed 1: %s at %d of %d, %lld.%lld s used of %lld s, %d lane changes,"
                    " %d of %d cars dropped\n",
                    lv.name, result, g.distance(), race::track_len, used / 1000, used % 1000 / 100,
                    static_cast<long long>(race::time_limit.count() / 1000), steers,
                    g.road().cars_dropped(), g.road().cars_drawn());
    }
}
