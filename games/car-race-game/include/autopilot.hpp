// autopilot.hpp - a computer driver that follows a planned crash-free route
// and uses all of its nitro. Used by the tests and the headless demo.
#ifndef RACE_AUTOPILOT_HPP
#define RACE_AUTOPILOT_HPP

#include "race.hpp"

#include <vector>

namespace race {

class Autopilot {
public:
    explicit Autopilot(const Game& g) : route_(plan_route(g.road(), g.lane())) {}

    bool has_route() const noexcept { return !route_.empty(); }

    Input next(const Game& g) const
    {
        if (!route_.empty()) {                       // steering comes first
            const int want = route_[static_cast<std::size_t>(g.distance()) + 1];
            if (want < g.lane()) return Input::left;
            if (want > g.lane()) return Input::right;
        }
        if (!g.boosting() && g.nitro() > 0)
            return Input::nitro;
        return Input::none;
    }

private:
    std::vector<int> route_;
};

}  // namespace race

#endif
