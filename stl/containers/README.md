# STL Containers in C++

[![stl-containers](https://github.com/mycplus/cpp-examples/actions/workflows/stl-containers.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/stl-containers.yml)

Source code for the MYCPLUS article
[STL Containers in C++: How to Choose the Right One](https://www.mycplus.com/programming/cpp/stl-containers/).

| File | What it shows |
| --- | --- |
| `src/list_operations.cpp` | `std::list` member operations: `remove`, `remove_if`, `unique`, `sort`, `merge`, `splice`, `reverse` |
| `src/list_sort_moves.cpp` | `list::sort` makes no element copies or moves and keeps iterators valid; `std::sort` on a vector moves values |
| `src/invalidation_tools.cpp` | Using an iterator or reference after a vector, deque or unordered_map grows; run under AddressSanitizer and `-D_GLIBCXX_DEBUG` |

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
bash tests/run_tests.sh "$PWD/build"      # add --portable for libc++, macOS or Windows
bash tests/sanitizers.sh g++              # Linux; also accepts clang++
```

C++17. Warnings are errors (`-Wall -Wextra -pedantic -Werror`, or `/W4 /WX` on MSVC).

## What the build checks

- **Linux (GCC and Clang, libstdc++):** builds each example, runs it and compares
  its output with `tests/expected/`, including the `std::sort` move count, which
  is specific to libstdc++.
- **Linux (Clang, libc++), macOS and Windows:** the same, minus that move count.
  Every library must report zero copies and moves for `list::sort`. Comparisons ignore `\r`.
- **Sanitizers (Linux, GCC and Clang):** AddressSanitizer reports the vector and
  deque-walk cases, libstdc++'s debug mode reports the vector, deque-iterator and
  deque-walk cases, and neither reports the valid deque reference. The cases no
  tool reports (deque iterator under ASan, unordered_map after rehash under both)
  are printed as INFO, not asserted: silence on undefined behaviour is not a
  property to test for.

## Licence

See the repository root.
