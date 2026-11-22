# Defensive Programming in C++

[![Defensive Programming](https://github.com/mycplus/cpp-examples/actions/workflows/defensive-programming.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/defensive-programming.yml)

The C++ version of the examples for [Defensive Programming in C: 10 Techniques with Tested Examples](https://www.mycplus.com/programming/c/defensive-programming-techniques/) on MYCPLUS. The C version is in [c-examples](https://github.com/mycplus/c-examples/tree/main/practices/defensive-programming).

| File | What it shows |
| --- | --- |
| `src/defensive.hpp`, `src/defensive.cpp` | `std::from_chars` parsing, a length-limited `getline`, an overflow-checked multiplication and a file copy that relies on RAII instead of `goto` |
| `src/people.cpp` | The C++ twin of `people.c`; it must print exactly the same report (`tests/expected/people.txt` is the same file) |
| `tests/test_defensive.cpp` | 53 checks, the same cases as the C tests where the two versions behave the same |
| `pitfalls/vector_index.cpp` | `at()` throws `std::out_of_range` where `operator[]` would read past the end |
| `compile_fail/ignored_status.cpp` | An ignored `[[nodiscard]]` status, which must not compile on GCC, Clang or MSVC |
| `run_all.sh` | GCC and Clang (C++17, C++20), ASan and UBSan, Valgrind and four deliberate mistakes |
| `run_all.ps1` | MSVC (C++17, C++20) and the same deliberate mistakes |

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

On Windows, run these in a Developer PowerShell: the compile-fail test calls `cl` directly.
