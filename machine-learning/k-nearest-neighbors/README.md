# K-Nearest Neighbors (KNN) in C++

[![K-Nearest Neighbors](https://github.com/mycplus/cpp-examples/actions/workflows/k-nearest-neighbors.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/k-nearest-neighbors.yml)

C++ companion for [K-Nearest Neighbors (KNN): How It Works, With Python and C++](https://www.mycplus.com/computer-science/ai/k-nearest-neighbors/) on MYCPLUS. The Python version is in [python-examples/machine-learning/k-nearest-neighbors](https://github.com/mycplus/python-examples/tree/main/machine-learning/k-nearest-neighbors). Both read the same `data/wine_split.csv` and are checked against the same `tests/expected/wine_knn.txt`, and both generate the same synthetic data from one splitmix64 stream.

| File | What it is |
| --- | --- |
| `src/knn.hpp` | Brute-force KNN in C++17: Euclidean or Manhattan distance, `nth_element` selection, majority vote, standardization, a CSV reader and the splitmix64 generator |
| `src/knn_wine.cpp` | The wine experiment, unscaled and standardized; prints exactly what the Python `wine_knn.py` prints |
| `src/knn_bench.cpp` | Times prediction on 20,000 synthetic training rows and 1,000 queries; its checksum must equal the Python benchmark's |
| `tests/test_knn.cpp` | The article's worked example, tie rules, `nearest()` against a full sort, standardization, CSV errors, and splitmix64 values shared with the Python tests |
| `tests/mutations.py` | Puts 14 deliberate mistakes into a copy of `knn.hpp` and checks each one is caught |
| `data/wine_split.csv` | The wine data with the article's train/test split |

## Data

[Wine](https://archive.ics.uci.edu/dataset/109/wine): Aeberhard, S. and Forina, M. (1992), UCI Machine Learning Repository, [doi:10.24432/C5PC7J](https://doi.org/10.24432/C5PC7J), licensed CC BY 4.0. `data/wine_split.csv` is written by the Python repository's `data.py`.

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
- CTest: the unit tests, `knn_wine` printing exactly `tests/expected/wine_knn.txt`, and `knn_bench` printing the Python benchmark's checksum.
- The same tests under AddressSanitizer and UndefinedBehaviorSanitizer.
- `tests/mutations.py` (Linux, GCC): all 14 deliberate mistakes are caught.
