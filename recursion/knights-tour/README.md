# Knight's Tour in C++

[![Knight's Tour](https://github.com/mycplus/cpp-examples/actions/workflows/knights-tour.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/knights-tour.yml)

Companion code for [Knight's Tour in C++](https://www.mycplus.com/computer-science/algorithms/knights-tour/) on MYCPLUS: Warnsdorff's rule, backtracking, and a checker, header-only C++17.

| File | What it is |
| --- | --- |
| `include/knights_tour.hpp` | `Board`, pure Warnsdorff (`warnsdorff`), backtracking search (`find_tour`), `is_tour` and `is_closed` |
| `src/knights_tour.cpp` | The article's program: `knights_tour [N [ROW COL]]` |
| `src/tour_stats.cpp` | Warnsdorff alone vs with backtracking from every start, 5 x 5 to 12 x 12 |
| `tests/` | Unit tests, expected outputs, rejected-argument tests |

## Build and run

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/knights_tour 8 0 0
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 on Linux and with MSVC `/W4 /WX /permissive-` on Windows, warnings as errors.
- `is_tour` accepts the original MYCPLUS program's sample board and rejects the broken board it printed for a start at (4, 2).
- Pure Warnsdorff with the original tie rule reproduces the original sample board exactly and gets stuck after 60 squares from (4, 2).
- Backtracking finds a valid tour from every possible start on boards 5 x 5 to 10 x 10, and proves no tour exists on 2 x 2 to 4 x 4 and from the wrong-colored squares of 5 x 5.
- `knights_tour` and `tour_stats` print exactly the files in `tests/expected/`; bad arguments are rejected.
- Everything runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.
