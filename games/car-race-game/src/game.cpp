// game.cpp - the race rules: steering, nitro, traffic, collisions, the clock.
#include "race.hpp"

#include <algorithm>

namespace race {

const char* const Car::sprite[car_h] = {" ^^ ", "[##]", "[##]"};
const char* const Obstacle::sprite[car_h] = {".--.", "|XX|", "'--'"};

bool Car::steer(int direction)
{
    const int to = lane_ + direction;
    if (to < 0 || to >= lanes)
        return false;
    lane_ = to;
    return true;
}

Game::Game(std::uint32_t seed, Level level, Traffic mode)
    : road_(seed, density_of(level), mode)
{
}

bool Game::hits_player(int lane) const
{
    return std::any_of(traffic_.begin(), traffic_.end(), [lane](const Obstacle& o) {
        return o.lane() == lane && o.overlaps_rows(car_top, car_top + car_h - 1);
    });
}

unsigned Game::step(Input in)
{
    if (over_)
        return 0;
    unsigned events = 0;

    if (in == Input::nitro)
        boost_ = !boost_ && nitro_ > 0;
    else if (in == Input::left || in == Input::right) {
        // Steering into a lane where a car is level with you is a side impact.
        if (car_.steer(in == Input::left ? -1 : 1) && hits_player(car_.lane()))
            events |= event::crash;
    }

    last_tick_ = boost_ ? boost_tick : normal_tick;
    if (boost_ && --nitro_ == 0)
        boost_ = false;
    timer_.tick(last_tick_);

    for (Obstacle& o : traffic_)
        o.advance();
    traffic_.erase(std::remove_if(traffic_.begin(), traffic_.end(),
                                  [](const Obstacle& o) { return o.off_screen(); }),
                   traffic_.end());
    ++distance_;
    const unsigned spawns = road_.spawns_at(distance_);
    for (int l = 0; l < lanes; ++l)
        if ((spawns >> l) & 1u)
            traffic_.emplace_back(l, -car_h);      // just above the screen

    if (hits_player(car_.lane()))
        events |= event::crash;
    if (events & event::crash)
        over_ = true;
    else if (distance_ >= track_len)
        events |= event::finish, over_ = true;
    else if (timer_.expired())
        events |= event::time_up, over_ = true;
    return events;
}

std::string Game::render() const
{
    const std::string border = '+' + std::string(road_w, '-') + "+\n";
    std::vector<std::string> rows(view_h, std::string(road_w, ' '));

    for (int y = 0; y < view_h; ++y) {
        // Dashed lane dividers that scroll with the road.
        const bool dash = (y - distance_ % 4 + 4) % 4 < 2;
        for (int l = 1; l < lanes; ++l)
            rows[y][l * (lane_w + 1) - 1] = dash ? ':' : ' ';
    }
    const int finish_row = car_top + distance_ - track_len;   // reaches the car at the end
    if (finish_row >= 0 && finish_row < view_h)
        for (int x = 0; x < road_w; ++x)
            rows[finish_row][x] = x % 2 ? ' ' : '#';

    auto draw = [&rows](int lane, int top, const char* const* sprite) {
        const int left = lane * (lane_w + 1) + 1;
        for (int r = 0; r < car_h; ++r)
            if (top + r >= 0 && top + r < view_h)
                rows[top + r].replace(left, car_w, sprite[r]);
    };
    for (const Obstacle& o : traffic_)
        draw(o.lane(), o.y(), Obstacle::sprite);
    draw(car_.lane(), car_top, Car::sprite);

    std::string frame = border;
    for (const std::string& row : rows)
        frame += '|' + row + "|\n";
    return frame + border;
}

}  // namespace race
