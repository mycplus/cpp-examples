# Knapsack Problem in C++

Companion code for [Knapsack Problem in C and C++: 0/1 Dynamic Programming, Item Recovery and the Fractional Variant](https://www.mycplus.com/computer-science/algorithms/solving-the-knapsack-problem/) on MYCPLUS.
The C versions, including the fractional variant and the pitfall programs, are in
[mycplus/c-examples](https://github.com/mycplus/c-examples/tree/main/dynamic-programming/knapsack).

| File | What it is |
| --- | --- |
| `src/knapsack.cpp` | 0/1 knapsack in C++17: `knapsack()` returns the best value and the chosen item indices, and throws on invalid input |
| `tests/compare_output.cmake` | runs a program and compares its output with a file in `expected/`, ignoring carriage returns |
| `tests/test_knapsack.cpp` | edge cases and 20,000 random instances checked against exhaustive search |

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

`tests/test_knapsack.cpp` includes `src/knapsack.cpp` directly and renames its `main`,
so the article's listing stays one complete program.
