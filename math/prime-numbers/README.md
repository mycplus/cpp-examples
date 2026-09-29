# Prime numbers in C++

[![prime-numbers](https://github.com/mycplus/cpp-examples/actions/workflows/prime-numbers.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/prime-numbers.yml)

Companion code for [Prime Number Programs in C, C++, Java, Python, C#, PHP and JavaScript](https://www.mycplus.com/computer-science/algorithms/prime-number-program/) on MYCPLUS: a primality test by trial division up to the square root, and the Sieve of Eratosthenes. The same program exists in seven languages and every version prints the same output.

| File | What it is |
| --- | --- |
| `src/primes.cpp` | `constexpr is_prime()` (with `static_assert` checks), a `std::vector<bool>` sieve and the demo |
| `tests/` | Unit tests and expected output |

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 and with MSVC, warnings as errors; the `static_assert`s run at compile time.
- Trial division agrees with the sieve on every number below 200,000.
- Known primes (including 2147483647, 1000000007 and 999999999989) and composites (including negatives, 0, 1, squares of primes and Carmichael numbers 561 and 1105) are classified correctly.
- The sieve reproduces the published prime counts: 168 below 1,000, 78,498 below 1,000,000 and 664,579 below 10,000,000.
- The demo prints exactly `tests/expected/primes.txt`, the same file in all seven repositories.
- Tests and demo run clean under AddressSanitizer and UndefinedBehaviorSanitizer.
