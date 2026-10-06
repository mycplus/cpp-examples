# Towers of Hanoi in C++

[![Towers of Hanoi](https://github.com/mycplus/cpp-examples/actions/workflows/towers-of-hanoi.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/towers-of-hanoi.yml)

Companion code for [Towers of Hanoi: Recursive Algorithm With Code in C, C++, Java, Python and C#](https://www.mycplus.com/computer-science/algorithms/towers-of-hanoi/) on MYCPLUS.

| File | What it is |
| --- | --- |
| `src/towers_of_hanoi.cpp` | The article's listing: prints the moves for 3 disks |
| `include/hanoi.hpp` | Header-only: `hanoi::solve` and `hanoi::solve_iterative` with a callback per move, `hanoi::move_at`, `hanoi::move_count` and `hanoi::moves` |
| `src/hanoi_cli.cpp` | `hanoi_cli N [--iterative]` prints the moves for 0 to 20 disks and rejects anything else |
| `pitfalls/print_in_one_statement.cpp` | Calling the printing solver inside the `std::cout` statement that prints the total |
| `tests/test_hanoi.cpp` | Simulates every move for 0 to 20 disks, compares the two solvers, checks `move_at` for 64 disks against an independent reference, and checks the boundaries |

Requires C++17.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```
