// race.hpp - rules for a terminal car race game. No input or output here.
#ifndef RACE_RACE_HPP
#define RACE_RACE_HPP

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace race {

inline constexpr int lanes = 3;
inline constexpr int lane_w = 6;                       // columns per lane
inline constexpr int road_w = lanes * lane_w + (lanes - 1);   // 20, with dividers
inline constexpr int view_h = 18;                      // road rows on screen
inline constexpr int car_h = 3, car_w = 4;             // every vehicle is 3 x 4
inline constexpr int car_top = 14;                     // player's top row
inline constexpr int track_len = 600;                  // rows from start to finish
inline constexpr int quiet_ticks = 20;                 // no new traffic near the end
inline constexpr int nitro_ticks = 200;                // boosted ticks in the tank

inline constexpr std::chrono::milliseconds normal_tick{70};
inline constexpr std::chrono::milliseconds boost_tick{35};
inline constexpr std::chrono::milliseconds time_limit{39000};

// A car spawned at tick s overlaps the player's rows on ticks s+first_hit
// through s+last_hit (both inclusive).
inline constexpr int first_hit = car_top + 1;                    // 15
inline constexpr int last_hit = car_top + car_h - 1 + car_h;     // 19

enum class Input { none, left, right, nitro };

namespace event {
inline constexpr unsigned crash = 1, finish = 2, time_up = 4;
}

// The player's car. It changes lane one lane at a time and never leaves the road.
class Car {
public:
    explicit Car(int lane) : lane_(lane) {}
    int lane() const noexcept { return lane_; }
    bool steer(int direction);               // -1 or +1; false if at the edge
    static const char* const sprite[car_h];

private:
    int lane_;
};

// A slower car in the player's way. It moves down one row per tick.
class Obstacle {
public:
    Obstacle(int lane, int y) : lane_(lane), y_(y) {}
    int lane() const noexcept { return lane_; }
    int y() const noexcept { return y_; }    // top row; negative above the screen
    void advance() noexcept { ++y_; }
    bool overlaps_rows(int top, int bottom) const noexcept
    {
        return y_ <= bottom && y_ + car_h - 1 >= top;
    }
    bool off_screen() const noexcept { return y_ >= view_h; }
    static const char* const sprite[car_h];

private:
    int lane_;
    int y_;
};

// Counts down game time, not wall-clock time: it only runs while the game
// runs, so pausing stops it and a test can replay a race exactly.
class Timer {
public:
    explicit Timer(std::chrono::milliseconds limit) : left_(limit) {}
    void tick(std::chrono::milliseconds dt) noexcept
    {
        left_ = dt >= left_ ? std::chrono::milliseconds{0} : left_ - dt;
    }
    std::chrono::milliseconds left() const noexcept { return left_; }
    bool expired() const noexcept { return left_.count() == 0; }

private:
    std::chrono::milliseconds left_;
};

enum class Traffic { naive, fair };

// Where traffic appears: a lane bitmask for every tick of the race, fixed by
// the seed before the race starts. The fair generator drops any car that
// would leave the player no way through.
class Road {
public:
    Road(std::uint32_t seed, int density_permille, Traffic mode);
    explicit Road(std::vector<unsigned char> spawns);   // for tests: index = tick

    unsigned spawns_at(int tick) const;            // bit l set: a car enters lane l
    bool blocked(int tick, int lane) const;        // a car overlaps the player's rows
    int cars_drawn() const noexcept { return drawn_; }
    int cars_dropped() const noexcept { return dropped_; }

private:
    std::vector<unsigned char> spawns_;            // index = tick, 1..track_len
    int drawn_ = 0, dropped_ = 0;
};

// Lanes the player can be in after `tick` without crashing, given the lanes
// reachable after the previous tick. The rules are the same as Game::step().
unsigned reachable_after(const Road& road, int tick, unsigned before);

// One crash-free lane for every tick 0..track_len, or empty if none exists.
std::vector<int> plan_route(const Road& road, int start_lane);

enum class Level { easy, normal, hard };
int density_of(Level level);                       // spawn chance per tick, in 1/1000

class Game {
public:
    Game(std::uint32_t seed, Level level, Traffic mode = Traffic::fair);
    explicit Game(Road road) : road_(std::move(road)) {}

    unsigned step(Input in);                       // one tick; returns event flags
    std::string render() const;                    // the road, one line per row

    const Road& road() const noexcept { return road_; }
    int lane() const noexcept { return car_.lane(); }
    int distance() const noexcept { return distance_; }
    int nitro() const noexcept { return nitro_; }
    bool boosting() const noexcept { return boost_; }
    bool over() const noexcept { return over_; }
    std::chrono::milliseconds time_left() const noexcept { return timer_.left(); }
    std::chrono::milliseconds last_tick() const noexcept { return last_tick_; }
    const std::vector<Obstacle>& traffic() const noexcept { return traffic_; }

private:
    bool hits_player(int lane) const;

    Road road_;
    Car car_{1};
    Timer timer_{time_limit};
    std::vector<Obstacle> traffic_;
    int distance_ = 0;
    int nitro_ = nitro_ticks;
    bool boost_ = false;
    bool over_ = false;
    std::chrono::milliseconds last_tick_ = normal_tick;
};

}  // namespace race

#endif
