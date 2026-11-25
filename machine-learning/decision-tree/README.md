# Decision Tree Inference in C++

[![Decision Tree](https://github.com/mycplus/cpp-examples/actions/workflows/decision-tree.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/decision-tree.yml)

C++ companion for [Decision Trees: Entropy, Information Gain and Gini, With Python and C++](https://www.mycplus.com/computer-science/ai/decision-tree/) on MYCPLUS. The tree is trained in Python ([python-examples/machine-learning/decision-tree](https://github.com/mycplus/python-examples/tree/main/machine-learning/decision-tree)) and used here two ways: loaded from a small text file at run time, and compiled in as C++ code generated from the same tree. Both are checked against the Python predictions.

| File | What it is |
| --- | --- |
| `src/tree.hpp` | Loads and checks `models/tree.txt`, walks the tree for a row, reads the test CSV, and the splitmix64 row order shared with the Python benchmark |
| `src/generated/breast_cancer_tree.hpp` | The same tree as one function of nested `if` statements, written by the Python `export_model.py` |
| `src/predict_tree.cpp` | Classifies the 114 test tumours; prints exactly what the Python `predict_exported.py` prints |
| `src/tree_bench.cpp` | Times the loaded tree and the generated function on 1,000,008 rows; its checksum must equal the Python benchmark's |
| `tests/test_tree.cpp` | Agreement with Python on the test rows, generated code against the loaded tree at every threshold boundary, malformed files, and the shared benchmark rows |
| `tests/mutations.py` | Puts 13 deliberate mistakes into a copy of the sources and checks each one is caught |
| `models/tree.txt`, `data/breast_cancer_test.csv` | The trained tree and the test rows, written by the Python repository |

## Data

[Breast Cancer Wisconsin (Diagnostic)](https://archive.ics.uci.edu/dataset/17/breast+cancer+wisconsin+diagnostic): Wolberg, Mangasarian, Street and Street (1993), UCI Machine Learning Repository, [doi:10.24432/C5DW2B](https://doi.org/10.24432/C5DW2B), licensed CC BY 4.0. Labels: 1 = malignant, 0 = benign.

## Build and test

CMake 3.16 or later and a C++17 compiler.

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

`bash run_all.sh` (Linux or WSL) runs every check behind the article: GCC and Clang with C++17 and C++20, AddressSanitizer and UndefinedBehaviorSanitizer, Valgrind, the deliberate mistakes and the benchmark. On Windows, `.\run_all.ps1` in a Developer PowerShell builds with MSVC (`/W4 /WX /permissive-`), runs CTest and the benchmark, and writes `results-windows.md`.

## What the build checks

- GCC and Clang, C++17 and C++20, with `-Wall -Wextra -pedantic -Werror`; MSVC with `/W4 /WX /permissive-`.
- CTest: the unit tests, `predict_tree` printing exactly `tests/expected/predict_exported.txt`, and `tree_bench` printing the Python benchmark's checksum.
- The same tests under AddressSanitizer and UndefinedBehaviorSanitizer.
- `tests/mutations.py` (Linux, GCC): all 13 deliberate mistakes are caught.
