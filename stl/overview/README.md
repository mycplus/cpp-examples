# The C++ Standard Template Library (STL)

[![stl-overview](https://github.com/mycplus/cpp-examples/actions/workflows/stl-overview.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/stl-overview.yml)

Source code for the MYCPLUS article on the C++ Standard Template Library:
containers, iterators, algorithms and function objects. Each program in `src/`
is self-contained and matches a listing in the article.

| File | What it shows |
| --- | --- |
| `src/first_look.cpp` | A container, iterators and three algorithms working together |
| `src/containers.cpp` | One example from each container family |
| `src/adaptors.cpp` | `stack`, `queue`, `priority_queue` and a min-heap |
| `src/lookup_cost.cpp` | Comparisons needed to find one value among 1,000,000, per container and algorithm |
| `src/stable_positions.cpp` | An index, `reserve()` and `std::list` as ways to survive vector growth |
| `src/erase_remove.cpp` | `std::remove` leaves the size unchanged; erase-remove and C++20 `std::erase` |
| `src/map_lookup.cpp` | `operator[]` inserts missing keys; `find()` and `at()` do not |
| `src/function_objects.cpp` | Lambdas, `std::greater` and a custom ordering for a `std::set` |
| `src/ranges_pipeline.cpp` | C++20 range algorithms and lazy views |
| `src/stream_iterators.cpp` | `istream_iterator`, `ostream_iterator` and `back_inserter` with algorithms |
| `src/algorithm_counts.cpp` | Predicate and comparison counts for `count_if`, `find_if` and `sort` |
| `src/modern_adapters.cpp` | Replacements for the removed C++98 adapters: `not_fn`, lambdas, `mem_fn`, `bind` |
| `tests/compile-fail/sort_list.cpp` | Must not compile: `std::sort` needs random-access iterators |
| `tests/compile-fail/ranges_sort_list.cpp` | The same mistake with `std::ranges::sort` |
| `tests/sanitizer-fail/invalidated_iterator.cpp` | An iterator used after `push_back` reallocates; AddressSanitizer reports it |

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
bash tests/run_tests.sh "$PWD/build"      # add --portable for libc++, macOS or Windows
bash tests/sanitizers.sh g++              # Linux; also accepts clang++
```

C++17, except `erase_remove.cpp` and `ranges_pipeline.cpp`, which are built as
C++20. Warnings are errors (`-Wall -Wextra -pedantic -Werror`, or `/W4 /WX` on MSVC).

## What the build checks

- **Linux (GCC and Clang, libstdc++):** builds every example, runs it and
  compares its output with `tests/expected/`, including the comparison counts
  in `lookup_cost.txt` and the `sort` comparison count in `algorithm_counts.txt`,
  which depend on the library's implementation.
- **Linux (Clang, libc++), macOS (Apple Clang) and Windows (MSVC):** build with
  warnings as errors and compare the outputs every conforming library must
  produce. For `lookup_cost` that is the linear-search count; for
  `algorithm_counts` it is the exact `count_if` and `find_if` counts; for
  `erase_remove` it is the result after `erase`, since the elements
  `std::remove` leaves behind are unspecified. Comparisons ignore `\r`.
- **Sanitizers (Linux, GCC and Clang):** the examples produce no
  AddressSanitizer or UndefinedBehaviorSanitizer report; the invalidated
  iterator is reported as `heap-use-after-free`; both compile-fail tests are
  rejected, and the ranges version's error names `random_access`.

## Licence

See the repository root.
