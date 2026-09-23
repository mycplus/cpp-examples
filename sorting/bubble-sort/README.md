# Bubble sort in C++

[![bubble-sort](https://github.com/mycplus/cpp-examples/actions/workflows/bubble-sort.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/bubble-sort.yml)

Companion code for [Bubble Sort in C and C++](https://www.mycplus.com/computer-science/algorithms/bubble-sort/) on MYCPLUS.

| File | What it is |
| --- | --- |
| `src/bubble_sort.cpp` | A generic `bubble_sort(first, last, comp)` for forward iterators, with a demo |
| `tests/test_bubble_sort.cpp` | Compares the result with `std::stable_sort` on `std::vector`, `std::forward_list`, `std::list` and a plain array |

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 with `-Wall -Wextra -pedantic -Werror`, and with MSVC under `/W4 /WX /permissive-`.
- On 5,000 random inputs with many equal keys, `bubble_sort()` produces exactly the order `std::stable_sort` produces, on all three container types. That checks stability, not only sortedness.
- The demo prints exactly the output shown in the article.
- The test runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.
