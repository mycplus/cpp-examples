// road_stats.cpp - how often does random traffic make a race impossible?
// For 1,000 seeds at each density, plans a route through the naive road and
// the fair road. A road with no route cannot be finished by any driver.
#include "race.hpp"

#include <cstdio>

int main()
{
    constexpr unsigned seeds = 1000;
    std::printf("Spawn chance  Cars/race  Naive: no route  Fair: no route  Fair: cars dropped\n");
    for (int density : {60, 100, 160, 220}) {
        int naive_stuck = 0, fair_stuck = 0;
        long drawn = 0, dropped = 0;
        for (unsigned seed = 1; seed <= seeds; ++seed) {
            const race::Road naive(seed, density, race::Traffic::naive);
            const race::Road fair(seed, density, race::Traffic::fair);
            naive_stuck += race::plan_route(naive, 1).empty();
            fair_stuck += race::plan_route(fair, 1).empty();
            drawn += fair.cars_drawn();
            dropped += fair.cars_dropped();
        }
        std::printf("%9.1f%%  %9.1f  %10u of %u  %9u of %u  %13.1f%%\n", density / 10.0,
                    static_cast<double>(drawn) / seeds, naive_stuck, seeds, fair_stuck, seeds,
                    100.0 * static_cast<double>(dropped) / static_cast<double>(drawn));
    }

    // The first trap on the naive road for seed 1 at normal density.
    const race::Road road(1, race::density_of(race::Level::normal), race::Traffic::naive);
    unsigned reach = 1u << 1;
    for (int t = 1; t <= race::track_len; ++t) {
        const unsigned next = race::reachable_after(road, t, reach);
        if (next != 0) {
            reach = next;
            continue;
        }
        std::printf("\nSeed 1, naive: no lane is safe at tick %d. Cars that block it:\n", t);
        for (int s = t - race::last_hit; s <= t - race::first_hit; ++s)
            for (int l = 0; l < race::lanes; ++l)
                if ((road.spawns_at(s) >> l) & 1u)
                    std::printf("  lane %d, spawned at tick %d, level with the player on ticks %d-%d\n",
                                l, s, s + race::first_hit, s + race::last_hit);
        break;
    }
}
