// Tests for the game rules in src/game.cpp and the autopilot.
#include "autopilot.hpp"
#include "game.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>

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

using brick::Game;
using brick::Input;
namespace ev = brick::event;

// A board with one brick far away and the ball placed by hand.
Game empty_board(int x, int y, int dx, int dy)
{
    Game g(1);
    g.clear_bricks();
    g.set_brick(brick::brick_rows - 1, 0, true);
    g.set_ball(x, y, dx, dy);
    return g;
}

int count_bricks(const Game& g)
{
    int n = 0;
    for (int r = 0; r < brick::brick_rows; ++r)
        for (int c = 0; c < brick::brick_cols; ++c)
            n += g.has_brick(r, c);
    return n;
}

void check_invariants(const Game& g, int lost)
{
    CHECK(g.ball_x() >= 0 && g.ball_x() < brick::board_w);
    CHECK(g.ball_y() >= 0 && g.ball_y() < brick::board_h);
    const int row = g.ball_y() - brick::brick_top;
    CHECK(!(row >= 0 && row < brick::brick_rows && g.has_brick(row, g.ball_x() / brick::brick_w)));
    CHECK(!(g.ball_y() == brick::board_h - 1 && g.ball_x() >= g.paddle_x() &&
            g.ball_x() < g.paddle_x() + brick::paddle_w));
    CHECK(count_bricks(g) == g.bricks_left());
    CHECK(g.balls() == brick::start_balls - lost);
}

void test_walls_and_corner()
{
    Game g = empty_board(0, 10, -1, -1);
    CHECK((g.step(Input::none) & ev::wall) && g.dx() == 1 && g.ball_x() == 1);
    g = empty_board(30, 0, 1, -1);
    CHECK((g.step(Input::none) & ev::wall) && g.dy() == 1 && g.ball_y() == 1);
    g = empty_board(0, 0, -1, -1);
    g.step(Input::none);
    CHECK(g.dx() == 1 && g.dy() == 1 && g.ball_x() == 1 && g.ball_y() == 1);
}

void test_corner_squeeze()
{
    Game g = empty_board(2, brick::brick_top + 1, 1, -1);
    g.set_brick(1, 1, true);          // beside the ball
    g.set_brick(0, 0, true);          // above it; the diagonal cell is empty
    CHECK(g.step(Input::none) & ev::brick);
    CHECK(!g.has_brick(1, 1) && !g.has_brick(0, 0));
    CHECK(g.score() == 2 * brick::brick_points && g.dx() == -1 && g.dy() == 1);
    CHECK(g.ball_x() == 1 && g.ball_y() == brick::brick_top + 2);
}

void test_paddle_aims()
{
    for (int offset = 0; offset < brick::paddle_w; ++offset) {
        Game g = empty_board(20 + offset, brick::board_h - 2, 1, 1);
        g.set_paddle(20);
        const int before = g.dx();
        CHECK(g.step(Input::none) & ev::paddle);
        CHECK(g.dy() == -1);
        if (offset < 2) CHECK(g.dx() == -1);
        else if (offset > brick::paddle_w - 3) CHECK(g.dx() == 1);
        else CHECK(g.dx() == before);
    }
}

void test_lost_ball_game_over_and_win()
{
    Game g = empty_board(40, brick::board_h - 1, 1, 1);
    g.set_paddle(0);
    g.set_score(15);
    CHECK(g.step(Input::none) & ev::ball_lost);
    CHECK(g.balls() == brick::start_balls - 1 && g.score() == 0);
    unsigned last = 0;
    for (int lost = 1; lost < brick::start_balls; ++lost) {
        g.set_ball(40, brick::board_h - 1, 1, 1);
        g.set_paddle(0);
        last = g.step(Input::none);
    }
    CHECK((last & ev::game_over) && g.over() && g.step(Input::none) == 0);

    Game w = empty_board(1, brick::brick_top + brick::brick_rows, 1, -1);
    CHECK((w.step(Input::none) & ev::win) && w.over() && w.bricks_left() == 0);
}

// Same seeds, same tick counts as the C version of this game.
void test_autopilot_games()
{
    const struct { std::uint32_t seed; long ticks; } expect[] = {{1, 3703}, {2, 2128}};
    for (const auto& e : expect) {
        Game g(e.seed);
        long ticks = 0;
        while (!g.over() && ticks < 100000) {
            g.step(brick::autopilot(g));
            ++ticks;
            if (!g.over())
                check_invariants(g, 0);
        }
        CHECK(g.bricks_left() == 0 && g.score() == 500 && ticks == e.ticks);
    }
}

void test_random_play_invariants()
{
    std::uint64_t rng = 88172645463325252ULL;
    for (std::uint32_t seed = 1; seed <= 300 && failures < 20; ++seed) {
        Game g(seed);
        int lost = 0;
        for (long t = 0; !g.over() && t < 200000; ++t) {
            rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
            const unsigned e = g.step(static_cast<Input>(rng % 3));
            lost += (e & ev::ball_lost) != 0;
            if (!g.over())
                check_invariants(g, lost);
        }
        CHECK(g.over());
    }
}

void test_render()
{
    const Game g(1);
    const std::string frame = g.render();
    CHECK(frame.size() == static_cast<std::size_t>((brick::board_w + 3) * (brick::board_h + 1)));
    CHECK(frame.rfind("+---", 0) == 0 && frame.find("|[#][#]") != std::string::npos);
    CHECK(frame.find('O') != std::string::npos && frame.find("=======") != std::string::npos);
}

}  // namespace

int main()
{
    test_walls_and_corner();
    test_corner_squeeze();
    test_paddle_aims();
    test_lost_ball_game_over_and_win();
    test_autopilot_games();
    test_random_play_invariants();
    test_render();
    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "game: all tests passed\n";
    return EXIT_SUCCESS;
}
