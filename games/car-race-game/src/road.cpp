// road.cpp - traffic generation and the route planner.
#include "race.hpp"

#include <random>
#include <utility>

namespace race {

namespace {
bool in_lane_mask(unsigned mask, int lane) { return (mask >> lane) & 1u; }
}  // namespace

int density_of(Level level)
{
    switch (level) {
    case Level::easy:   return 100;
    case Level::normal: return 160;
    case Level::hard:   return 220;
    }
    return 160;
}

Road::Road(std::uint32_t seed, int density_permille, Traffic mode)
    : spawns_(track_len + 1, 0)
{
    // std::mt19937's output is fixed by the standard; the distributions in
    // <random> are not, so the mapping to lanes and chances is done by hand.
    std::mt19937 rng(seed);
    std::vector<unsigned> reach(track_len + 1, 0);
    reach[0] = 1u << 1;                            // the player starts in lane 1
    int known = 0;                                 // reach[] is final up to here

    for (int t = 1; t <= track_len; ++t) {
        const bool spawn = static_cast<int>(rng() % 1000) < density_permille;
        const int lane = static_cast<int>(rng() % lanes);
        if (!spawn || t > track_len - quiet_ticks)
            continue;
        ++drawn_;
        spawns_[t] = static_cast<unsigned char>(1u << lane);
        if (mode == Traffic::naive)
            continue;

        // A car spawned at t first reaches the player at t + first_hit, and
        // nothing spawned later can affect earlier ticks. So the reachable
        // lanes up to t + first_hit are now final: check that one is left.
        const int horizon = t + first_hit;
        if (horizon > track_len)
            continue;
        for (int k = known + 1; k < horizon; ++k)
            reach[k] = reachable_after(*this, k, reach[k - 1]);
        known = horizon - 1;
        if (reachable_after(*this, horizon, reach[horizon - 1]) == 0) {
            spawns_[t] = 0;                        // this car would trap the player
            ++dropped_;
        }
    }
}

Road::Road(std::vector<unsigned char> spawns) : spawns_(std::move(spawns))
{
    spawns_.resize(track_len + 1, 0);
    spawns_[0] = 0;
}

unsigned Road::spawns_at(int tick) const
{
    return tick >= 1 && tick <= track_len ? spawns_[tick] : 0u;
}

bool Road::blocked(int tick, int lane) const
{
    for (int s = tick - last_hit; s <= tick - first_hit; ++s)
        if (in_lane_mask(spawns_at(s), lane))
            return true;
    return false;
}

unsigned reachable_after(const Road& road, int tick, unsigned before)
{
    unsigned after = 0;
    for (int to = 0; to < lanes; ++to) {
        if (road.blocked(tick, to))
            continue;                              // hit from the front
        for (int from = to - 1; from <= to + 1; ++from) {
            if (from < 0 || from >= lanes || !in_lane_mask(before, from))
                continue;
            if (from != to && road.blocked(tick - 1, to))
                continue;                          // steered into the side of a car
            after |= 1u << to;
        }
    }
    return after;
}

std::vector<int> plan_route(const Road& road, int start_lane)
{
    std::vector<unsigned> reach(track_len + 1, 0);
    reach[0] = 1u << start_lane;
    for (int t = 1; t <= track_len; ++t) {
        reach[t] = reachable_after(road, t, reach[t - 1]);
        if (reach[t] == 0)
            return {};
    }
    // Walk back from the finish, choosing at each tick a lane the next one
    // can be reached from, and preferring not to change lane.
    std::vector<int> route(track_len + 1, 0);
    for (int l = 0; l < lanes; ++l)
        if (in_lane_mask(reach[track_len], l)) {
            route[track_len] = l;
            break;
        }
    for (int t = track_len; t > 0; --t) {
        const int to = route[t];
        for (int from : {to, to - 1, to + 1}) {
            if (from >= 0 && from < lanes && in_lane_mask(reach[t - 1], from) &&
                in_lane_mask(reachable_after(road, t, 1u << from), to)) {
                route[t - 1] = from;
                break;
            }
        }
    }
    return route;
}

}  // namespace race
