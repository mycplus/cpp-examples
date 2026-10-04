# Library Sorts in C++

`std::sort`, `std::ranges::sort`, `std::stable_sort` and `qsort` compared on the same data:
comparison counts, stability on equal keys, and timings. Companion code for
[Sorting Algorithms in C and C++](https://www.mycplus.com/computer-science/algorithms/sorting-algorithms/).

| File | What it does |
| --- | --- |
| `src/library_sorts.cpp` | comparisons on 10,000 ints, a stability check on 1,000 records, median-of-5 timings on 1,000,000 ints |
| `src/stable_vs_sort.cpp` | the short example on the article page: where `std::sort` and `std::stable_sort` put equal keys |
| `tests/test_library_sorts.cpp` | `std::stable_sort` against a (key, position) reference on 5,000 random inputs |
| `pitfalls/lte_comparator.cpp` | `std::sort` with `<=`: undefined behavior that prints a normal result in an optimized build |

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The workflow also checks that the `<=` comparator is rejected by libstdc++ debug mode
(`comparison doesn't meet irreflexive requirements`) and reported by AddressSanitizer
(`heap-buffer-overflow`). Timings are printed but not checked.
