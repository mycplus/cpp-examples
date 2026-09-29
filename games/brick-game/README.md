# Brick breaker game in C++

[![brick-game](https://github.com/mycplus/cpp-examples/actions/workflows/brick-game.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/brick-game.yml)

Companion code for [Brick Breaker Game in C and C++](https://www.mycplus.com/programming/c/brick-game/) on MYCPLUS. The same game as the C version in [mycplus/c-examples](https://github.com/mycplus/c-examples/tree/main/games/brick-game), written in C++17.

| File | What it is |
| --- | --- |
| `include/game.hpp`, `src/game.cpp` | `brick::Game`: the rules, with no I/O |
| `include/screen.hpp`, `src/screen.cpp` | Full and changed-cells-only redraws |
| `include/terminal.hpp`, `src/terminal.cpp` | `brick::Terminal`: raw input and output; the destructor restores the terminal |
| `src/main.cpp` | The game loop, timed with `std::chrono::steady_clock` |
| `include/autopilot.hpp`, `src/autoplay.cpp` | A computer player and a headless demo |
| `tests/` | Rule tests, a screen test with a small terminal emulator, a pseudo-terminal play test, expected output |

## Build and play

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/brick advanced        # novice (default), advanced or expert
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 on Linux, with Clang on macOS, and with MSVC `/W4 /WX /permissive-` on Windows.
- The same rule tests as the C version, including the tick counts of the two autopilot games.
- `autoplay` prints byte-for-byte the same output as the C version's `autoplay`.
- The screen diff reproduces every frame of a full game in a small terminal emulator.
- On Linux and macOS, the pseudo-terminal test checks that Q and Ctrl+C both exit with status 0 and restore the cursor.
- Tests and `autoplay` run clean under AddressSanitizer and UndefinedBehaviorSanitizer.
