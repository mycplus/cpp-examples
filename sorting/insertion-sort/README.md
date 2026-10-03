# Insertion Sort in C++

Companion code for [Insertion Sort in C, C++, Java, Python and C#](https://www.mycplus.com/computer-science/algorithms/insertion-sort/) on MYCPLUS.

| File | What it is |
| --- | --- |
| `src/insertion_demo.cpp` | the generic insertion sort on four containers |
| `src/insertion_sort.hpp` | generic insertion sort for C++17 |
| `tests/compare_output.cmake` | runs a program and compares its output with a file in `expected/`, ignoring carriage returns |
| `tests/test_insertion_sort.cpp` | both sorts against std::stable_sort on 5,000 random inputs of (key, position) pairs, in std::vector and std::list. |

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The tests compare each program's output with `tests/expected/` and check the sort
against a reference on thousands of random inputs. The programs in `pitfalls/` are
deliberately wrong; the article explains what each one does. Some are built and run by the
tests (their output is deterministic); the rest are checked by the workflow, which
confirms the compiler warning or sanitizer report the article describes.
