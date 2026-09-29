// game.hpp - game rules for a terminal brick game. No I/O.
#ifndef BRICK_GAME_HPP
#define BRICK_GAME_HPP

#include <array>
#include <cstdint>
#include <string>

namespace brick {

inline constexpr int board_w = 60;              // playfield columns inside the walls
inline constexpr int board_h = 20;              // rows; the paddle is on the last
inline constexpr int brick_w = 3;
inline constexpr int brick_cols = board_w / brick_w;   // 20 bricks per row
inline constexpr int brick_rows = 5;
inline constexpr int brick_top = 2;             // rows 0-1 are open space
inline constexpr int paddle_w = 7;
inline constexpr int paddle_step = 2;
inline constexpr int start_balls = 3;
inline constexpr int brick_points = 5;
inline constexpr int lost_ball_penalty = 20;

enum class Input { none, left, right };

// Bit flags returned by Game::step().
namespace event {
inline constexpr unsigned wall = 1, paddle = 2, brick = 4,
                          ball_lost = 8, win = 16, game_over = 32;
}

class Game {
public:
    explicit Game(std::uint32_t seed);

    unsigned step(Input in);            // advance one tick; returns event flags
    std::string render() const;         // the playfield as text, one line per row

    int score() const noexcept { return score_; }
    int balls() const noexcept { return balls_; }
    int bricks_left() const noexcept { return bricks_left_; }
    bool over() const noexcept { return over_; }
    int ball_x() const noexcept { return ball_x_; }
    int ball_y() const noexcept { return ball_y_; }
    int dx() const noexcept { return dx_; }
    int dy() const noexcept { return dy_; }
    int paddle_x() const noexcept { return paddle_x_; }
    bool has_brick(int row, int col) const { return bricks_.at(row).at(col); }

    // For tests: place the ball, the paddle and the bricks by hand.
    void set_ball(int x, int y, int dx, int dy);
    void set_paddle(int x) { paddle_x_ = x; }
    void set_brick(int row, int col, bool present);
    void clear_bricks();
    void set_score(int score) { score_ = score; }

private:
    enum class Cell { empty, wall, brick, paddle, out };

    Cell cell_at(int x, int y) const;
    void hit(Cell c, int x, int y, unsigned& events);
    void serve();
    std::uint32_t next_random();

    std::array<std::array<bool, brick_cols>, brick_rows> bricks_{};
    int bricks_left_ = brick_rows * brick_cols;
    int ball_x_ = 0, ball_y_ = 0, dx_ = 1, dy_ = -1;
    int paddle_x_ = (board_w - paddle_w) / 2;
    int score_ = 0;
    int balls_ = start_balls;
    std::uint32_t rng_;
    bool over_ = false;
};

}  // namespace brick

#endif
