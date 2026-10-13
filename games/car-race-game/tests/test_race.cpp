// Tests for the race rules (src/game.cpp), traffic generation and the route
// planner (src/road.cpp), and the autopilot.
#include "autopilot.hpp"
#include "race.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

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

using race::Input;
namespace ev = race::event;
using ms = std::chrono::milliseconds;

// A road with cars entering the given lanes at the given ticks.
race::Road scripted(std::vector<std::pair<int, int>> tick_lane)
{
    std::vector<unsigned char> spawns(race::track_len + 1, 0);
    for (auto [tick, lane] : tick_lane) {
        unsigned char& mask = spawns[static_cast<std::size_t>(tick)];
        mask = static_cast<unsigned char>(mask | (1u << lane));
    }
    return race::Road(spawns);
}

// Steps until the game reaches `distance`, applying `in` on the tick that
// reaches it. Returns the events of the last step.
unsigned drive_to(race::Game& g, int distance, Input in = Input::none)
{
    unsigned events = 0;
    while (!g.over() && g.distance() < distance - 1)
        events = g.step(Input::none);
    if (!g.over())
        events = g.step(in);
    return events;
}

void test_car_and_timer()
{
    race::Car car(1);
    CHECK(car.steer(-1) && car.lane() == 0);
    CHECK(!car.steer(-1) && car.lane() == 0);         // the edge of the road
    CHECK(car.steer(1) && car.steer(1) && !car.steer(1) && car.lane() == 2);

    race::Timer timer(ms(100));
    timer.tick(ms(70));
    CHECK(timer.left() == ms(30) && !timer.expired());
    timer.tick(ms(70));                               // never goes below zero
    CHECK(timer.left() == ms(0) && timer.expired());

    const race::Obstacle o(0, 12);
    CHECK(o.overlaps_rows(14, 16) && !race::Obstacle(0, 11).overlaps_rows(14, 16));
    CHECK(race::Obstacle(0, 16).overlaps_rows(14, 16) && !race::Obstacle(0, 17).overlaps_rows(14, 16));
}

void test_front_crash_timing()
{
    // A car entering the player's lane at tick 1 reaches the player at tick 16.
    race::Game g(scripted({{1, 1}}));
    CHECK(drive_to(g, 15) == 0);
    CHECK(drive_to(g, 16) == ev::crash && g.over());
    CHECK(g.road().blocked(16, 1) && !g.road().blocked(15, 1) && g.road().blocked(20, 1));
}

void test_dodge_and_side_impact()
{
    // Dodge at tick 15, then try to return while the car is level with us.
    for (int back : {17, 21, 22}) {
        race::Game g(scripted({{1, 1}}));
        CHECK(drive_to(g, 15, Input::left) == 0 && g.lane() == 0);
        const unsigned e = drive_to(g, back, Input::right);
        if (back == 22)
            CHECK(e == 0 && g.lane() == 1);           // the car has gone past
        else
            CHECK(e == ev::crash);                    // steered into its side
    }
}

void test_staircase_has_no_route()
{
    // One car per lane on three consecutive ticks: no lane is ever clear.
    const race::Road wall = scripted({{27, 0}, {28, 1}, {29, 2}});
    CHECK(race::plan_route(wall, 1).empty());
    const race::Road gap = scripted({{27, 0}, {28, 1}, {35, 2}});
    CHECK(!race::plan_route(gap, 1).empty());
}

void test_nitro_and_clock()
{
    race::Game g(scripted({}));
    CHECK(g.step(Input::nitro) == 0 && g.boosting() && g.last_tick() == ms(35));
    CHECK(g.nitro() == race::nitro_ticks - 1);
    g.step(Input::nitro);                             // toggles off
    CHECK(!g.boosting() && g.last_tick() == ms(70) && g.nitro() == race::nitro_ticks - 1);

    // Without nitro the race takes 600 x 70 ms = 42 s, more than the 39 s limit.
    race::Game slow(scripted({}));
    unsigned e = 0;
    while (!slow.over())
        e = slow.step(Input::none);
    CHECK(e == ev::time_up && slow.distance() == 558);

    // With the whole tank: 200 x 35 + 400 x 70 = 35 s.
    race::Game fast(scripted({}));
    fast.step(Input::nitro);
    while (!fast.over())
        e = fast.step(Input::none);
    CHECK(e == ev::finish && fast.nitro() == 0 && fast.time_left() == ms(39000 - 35000));
}

// The game's collisions (Obstacle positions) and the planner's model
// (Road::blocked) are written separately; random driving checks they agree.
void test_game_matches_model()
{
    std::mt19937 rng(7);
    for (int game = 0; game < 300; ++game) {
        race::Game g(static_cast<std::uint32_t>(game + 1), race::Level::hard, race::Traffic::naive);
        while (!g.over()) {
            const int before = g.lane();
            const auto in = static_cast<Input>(rng() % 4);
            const unsigned e = g.step(in);
            const bool safe = (race::reachable_after(g.road(), g.distance(), 1u << before) >> g.lane()) & 1u;
            if (((e & ev::crash) != 0) == safe) {
                CHECK(!"game and route planner disagree");
                return;
            }
            for (const race::Obstacle& o : g.traffic())
                if (o.y() >= race::view_h) {
                    CHECK(!"a car below the screen was kept");
                    return;
                }
        }
    }
}

void test_fair_roads_and_autopilot()
{
    for (auto level : {race::Level::easy, race::Level::normal, race::Level::hard}) {
        for (std::uint32_t seed = 1; seed <= 200; ++seed) {
            race::Game g(seed, level);
            const race::Autopilot pilot(g);
            CHECK(pilot.has_route());
            unsigned e = 0;
            while (!g.over())
                e = g.step(pilot.next(g));
            if (e != ev::finish || g.time_left() != ms(4000)) {
                CHECK(!"autopilot did not finish a fair road in 35 s");
                return;
            }
        }
    }
}

void test_render_shape()
{
    race::Game g(scripted({{1, 2}}));
    drive_to(g, 10);
    const std::string frame = g.render();
    std::size_t lines = 0, start = 0;
    while (start < frame.size()) {
        const std::size_t end = frame.find('\n', start);
        CHECK(end - start == race::road_w + 2);
        start = end + 1;
        ++lines;
    }
    CHECK(lines == race::view_h + 2);
    CHECK(frame.find("[##]") != std::string::npos && frame.find("|XX|") != std::string::npos);
}

}  // namespace

int main()
{
    test_car_and_timer();
    test_front_crash_timing();
    test_dodge_and_side_impact();
    test_staircase_has_no_route();
    test_nitro_and_clock();
    test_game_matches_model();
    test_fair_roads_and_autopilot();
    test_render_shape();
    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "rules: all tests passed\n";
    return EXIT_SUCCESS;
}
