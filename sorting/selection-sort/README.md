# Selection Sort in C++

Companion code for [Selection Sort in C, C++, Java, Python and C#](https://www.mycplus.com/computer-science/algorithms/selection-sort/) on MYCPLUS.

| File | What it is |
| --- | --- |
| `src/selection_demo.cpp` | the generic selection sort on three containers, and what it does to equal keys |
| `src/selection_sort.hpp` | generic selection sort for C++17 |
| `tests/compare_output.cmake` | runs a program and compares its output with a file in `expected/`, ignoring carriage returns |
| `tests/test_selection_sort.cpp` | against std::sort on 5,000 random inputs in std::vector and std::forward_list, comparing keys only (not stable). |

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
