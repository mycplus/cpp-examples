# Car race game in C++

[![car-race-game](https://github.com/mycplus/cpp-examples/actions/workflows/car-race-game.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/car-race-game.yml)

Companion code for [Car Race Game in C++](https://www.mycplus.com/programming/cpp/car-race-game/) on MYCPLUS. A three-lane terminal racing game in C++17: dodge traffic, use your nitro, and reach the finish line before the clock runs out. Runs in Windows Terminal and in Linux and macOS terminals.

| File | What it is |
| --- | --- |
| `include/race.hpp`, `src/game.cpp` | `Car`, `Obstacle`, `Timer` and `Game`: the rules, with no I/O |
| `src/road.cpp` | `Road`: traffic for the whole race, naive or fair, and the route planner |
| `include/screen.hpp`, `src/screen.cpp` | Full and changed-cells-only redraws |
| `include/terminal.hpp`, `src/terminal.cpp` | `race::Terminal`: raw input and output; the destructor restores the terminal |
| `src/main.cpp` | The game loop, timed with `std::chrono::steady_clock` |
| `include/autopilot.hpp`, `src/autoplay.cpp` | A computer driver and a headless demo |
| `src/road_stats.cpp` | How often random traffic makes a race impossible, naive vs fair |
| `src/random_check.cpp` | `std::mt19937` output (fixed by the standard) next to `std::uniform_int_distribution` output (not fixed); CI prints it on each platform |
| `tests/` | Rule tests, a screen test with a small terminal emulator, a pseudo-terminal play test, expected output |

## Build and play

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/race normal        # easy, normal (default) or hard
```

Left/Right or A/D steer, Up or W toggles nitro, P or Space pauses, Q quits.

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 on Linux, with Clang on macOS, and with MSVC `/W4 /WX /permissive-` on Windows.
- The rules: steering, front and side collisions, nitro, the clock, and agreement between the game's collisions and the route planner over 300 random-input races.
- Every fair road for seeds 1-200 at each level is finished by the autopilot.
- `autoplay` and `road_stats` print exactly the output in `tests/expected/`.
- The screen diff reproduces every frame of a full race in a small terminal emulator.
- On Linux and macOS, the pseudo-terminal test checks that Q and Ctrl+C both exit with status 0 and restore the cursor.
- Tests, `autoplay` and `road_stats` run clean under AddressSanitizer and UndefinedBehaviorSanitizer.
