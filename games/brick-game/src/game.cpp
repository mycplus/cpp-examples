// game.cpp - brick game rules: movement, collisions, scoring and rendering.
#include "game.hpp"

namespace brick {

Game::Game(std::uint32_t seed) : rng_(seed != 0 ? seed : 1)   // xorshift needs non-zero
{
    for (auto& row : bricks_)
        row.fill(true);
    serve();
}

std::uint32_t Game::next_random()                 // xorshift32
{
    rng_ ^= rng_ << 13;
    rng_ ^= rng_ >> 17;
    rng_ ^= rng_ << 5;
    return rng_;
}

void Game::serve()
{
    ball_x_ = paddle_x_ + paddle_w / 2;
    ball_y_ = board_h - 2;                        // just above the paddle
    dx_ = (next_random() & 1u) ? 1 : -1;
    dy_ = -1;
}

// Every array index is computed after the bounds checks.
Game::Cell Game::cell_at(int x, int y) const
{
    if (x < 0 || x >= board_w || y < 0)
        return Cell::wall;
    if (y >= board_h)
        return Cell::out;
    if (y == board_h - 1 && x >= paddle_x_ && x < paddle_x_ + paddle_w)
        return Cell::paddle;
    const int row = y - brick_top;
    if (row >= 0 && row < brick_rows && bricks_[row][x / brick_w])
        return Cell::brick;
    return Cell::empty;
}

void Game::hit(Cell c, int x, int y, unsigned& events)
{
    switch (c) {
    case Cell::brick:
        bricks_[y - brick_top][x / brick_w] = false;
        --bricks_left_;
        score_ += brick_points;
        events |= event::brick;
        break;
    case Cell::paddle:
        events |= event::paddle;
        break;
    case Cell::wall:
        events |= event::wall;
        break;
    case Cell::empty:
    case Cell::out:
        break;
    }
}

unsigned Game::step(Input in)
{
    unsigned events = 0;
    if (over_)
        return 0;

    if (in == Input::left)
        paddle_x_ -= paddle_step;
    else if (in == Input::right)
        paddle_x_ += paddle_step;
    if (paddle_x_ < 0)
        paddle_x_ = 0;
    if (paddle_x_ > board_w - paddle_w)
        paddle_x_ = board_w - paddle_w;

    const auto solid = [](Cell c) {
        return c == Cell::wall || c == Cell::brick || c == Cell::paddle;
    };
    const int x = ball_x_, y = ball_y_;

    // Check the two cells beside the ball's path before the diagonal one,
    // so the ball cannot slip between two blocks that touch at a corner.
    const Cell side = cell_at(x + dx_, y);
    const Cell vert = cell_at(x, y + dy_);
    if (solid(side)) {
        hit(side, x + dx_, y, events);
        dx_ = -dx_;
    }
    if (solid(vert)) {
        if (vert == Cell::paddle) {               // the paddle's ends aim
            const int offset = x - paddle_x_;
            if (offset < 2)
                dx_ = -1;
            else if (offset > paddle_w - 3)
                dx_ = 1;
        }
        hit(vert, x, y + dy_, events);
        dy_ = -dy_;
    }
    if (!solid(side) && !solid(vert)) {
        const Cell diag = cell_at(x + dx_, y + dy_);
        if (solid(diag)) {
            hit(diag, x + dx_, y + dy_, events);
            dx_ = -dx_;
            dy_ = -dy_;
        }
    }

    if (!solid(cell_at(ball_x_ + dx_, ball_y_ + dy_))) {   // move if free
        ball_x_ += dx_;
        ball_y_ += dy_;
    }

    if (bricks_left_ == 0) {
        over_ = true;
        return events | event::win;
    }
    if (ball_y_ >= board_h) {                     // fell past the paddle
        events |= event::ball_lost;
        score_ = score_ > lost_ball_penalty ? score_ - lost_ball_penalty : 0;
        if (--balls_ == 0) {
            over_ = true;
            return events | event::game_over;
        }
        serve();
    }
    return events;
}

std::string Game::render() const
{
    static constexpr char brick_art[] = "[#]";
    std::string frame;
    frame.reserve(static_cast<std::size_t>((board_w + 3) * (board_h + 1)));
    frame += '+';
    frame.append(board_w, '-');
    frame += "+\n";
    for (int y = 0; y < board_h; ++y) {
        frame += '|';
        for (int x = 0; x < board_w; ++x) {
            const Cell c = cell_at(x, y);
            if (x == ball_x_ && y == ball_y_ && !over_)
                frame += 'O';
            else if (c == Cell::paddle)
                frame += '=';
            else if (c == Cell::brick)
                frame += brick_art[x % brick_w];
            else
                frame += ' ';
        }
        frame += "|\n";
    }
    return frame;
}

void Game::set_ball(int x, int y, int dx, int dy)
{
    ball_x_ = x;
    ball_y_ = y;
    dx_ = dx;
    dy_ = dy;
}

void Game::set_brick(int row, int col, bool present)
{
    bool& b = bricks_.at(row).at(col);
    if (b != present)
        bricks_left_ += present ? 1 : -1;
    b = present;
}

void Game::clear_bricks()
{
    for (auto& row : bricks_)
        row.fill(false);
    bricks_left_ = 0;
}

}  // namespace brick
