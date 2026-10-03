# Heap Sort in C++

Companion code for [Heap Sort in C, C++, Java, Python and C#](https://www.mycplus.com/computer-science/algorithms/heap-sort/) on MYCPLUS.

| File | What it is |
| --- | --- |
| `src/heap_demo.cpp` | the generic heap sort, and the standard library's heap tools |
| `src/heap_sort.hpp` | generic heap sort for C++17 |
| `tests/compare_output.cmake` | runs a program and compares its output with a file in `expected/`, ignoring carriage returns |
| `tests/test_heap_sort.cpp` | against std::sort on 5,000 random inputs, with the default comparator and std::greater. |

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
