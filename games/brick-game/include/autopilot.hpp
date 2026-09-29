// autopilot.hpp - a simple computer player, used by the demo and the tests.
#ifndef BRICK_AUTOPILOT_HPP
#define BRICK_AUTOPILOT_HPP

#include "game.hpp"

namespace brick {

// Steers toward where a falling ball will reach the paddle row, ignoring
// bricks, so that the ball lands on the paddle's centre.
inline Input autopilot(const Game& g)
{
    if (g.dy() < 0)
        return Input::none;                             // ball is rising
    int x = g.ball_x(), dx = g.dx();
    for (int y = g.ball_y(); y < board_h - 2; ++y) {
        if (x + dx < 0 || x + dx >= board_w)
            dx = -dx;                                   // side wall
        x += dx;
    }
    const int target = x - paddle_w / 2;
    if (target < g.paddle_x())
        return Input::left;
    if (target > g.paddle_x())
        return Input::right;
    return Input::none;
}

}  // namespace brick

#endif
