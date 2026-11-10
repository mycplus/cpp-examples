# Longest Common Subsequence in C++

[![LCS](https://github.com/mycplus/cpp-examples/actions/workflows/lcs.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/lcs.yml)

Companion code for [Longest Common Subsequence in C and C++](https://www.mycplus.com/computer-science/algorithms/longest-common-subsequence/) on MYCPLUS: a generic LCS for any sequences that compare with `==` (strings, `std::vector<int>`, lines of text), its length in linear memory, and a line diff. The C version, with a diff program and more tests, is in [mycplus/c-examples](https://github.com/mycplus/c-examples/tree/main/dynamic-programming/lcs).

| File | What it is |
| --- | --- |
| `src/lcs.cpp` | `lcs_table`, `lcs_pairs`, `lcs`, `lcs_length` and `diff`, with a `main` that runs them |
| `tests/test_lcs.cpp` | 10,000 random pairs against brute force, generic sequences, 500 random diffs |

## Build and run

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/lcs
```

## What the build checks

- Compiles with GCC and Clang under C++17 and C++20 on Linux and with MSVC `/W4 /WX /permissive-` on Windows, warnings as errors.
- The tests pass, and the program prints exactly `tests/expected/lcs.txt`.
- On Linux, everything runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.
