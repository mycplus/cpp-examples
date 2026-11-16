# Linear Search in C++

[![Linear Search](https://github.com/mycplus/cpp-examples/actions/workflows/linear-search.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/linear-search.yml)

Companion code for [Linear Search in C and C++](https://www.mycplus.com/computer-science/algorithms/linear-search/) on MYCPLUS: a generic `find_index()` for any container, `std::find` and `std::find_if`, move-to-front with `std::rotate`, and `std::lower_bound` for sorted data, in C++17. The C version, with a sentinel search, pitfalls, a benchmark and mutation tests, is in [mycplus/c-examples](https://github.com/mycplus/c-examples/tree/main/searching/linear-search).

| File | What it is |
| --- | --- |
| `src/linear_search.cpp` | `find_index()`, `move_to_front()` and a `main` that runs the article's examples |
| `tests/test_linear_search.cpp` | Edge cases and 20,000 random vectors and lists against a reference |
| `run_all.sh`, `run_all.ps1` | Every check in one command, for Ubuntu/WSL and for Windows (MSVC) |

## Build and run

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/linear_search
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 on Linux and with MSVC `/W4 /WX /permissive-` on Windows, warnings as errors.
- 20,000 random vectors and lists agree with a reference that scans the other way, for both `find_index()` and `move_to_front()`.
- The program prints exactly `tests/expected/linear_search.txt`.
- On Linux: everything runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.
