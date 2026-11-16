# Pairwise Sequence Alignment in C++

[![Sequence Alignment](https://github.com/mycplus/cpp-examples/actions/workflows/sequence-alignment.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/sequence-alignment.yml)

Companion code for [Smith-Waterman and Needleman-Wunsch in C and C++](https://www.mycplus.com/computer-science/algorithms/smith-waterman-algorithm/) on MYCPLUS: optimal global (Needleman-Wunsch) and local (Smith-Waterman) alignment with affine gap costs in C++17, using Gotoh's three tables. The C version, with a FASTA command-line tool, pitfalls and a Biopython cross-check, is in [mycplus/c-examples](https://github.com/mycplus/c-examples/tree/main/dynamic-programming/sequence-alignment).

| File | What it is |
| --- | --- |
| `src/align.cpp` | `Scoring`, `align()`, `align_score()` (one row of cells) and `print()`, with a `main` that aligns the article's examples |
| `src/blosum.hpp` | The BLOSUM62 and BLOSUM50 protein scoring tables |
| `tests/test_align.cpp` | Known results, edge cases, 2,000 random pairs against a brute force, long protein pairs |
| `run_all.sh`, `run_all.ps1` | Every check in one command, for Ubuntu/WSL and for Windows (MSVC) |

A gap of length k costs `open + k * extend`.

## Build and run

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/align
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 on Linux and with MSVC `/W4 /WX /permissive-` on Windows, warnings as errors.
- Known textbook results; 2,000 random pairs with random, deliberately asymmetric scoring tables agree with a brute force over every alignment, global and local; the one-row score agrees with the full table on long protein pairs.
- The program prints exactly `tests/expected/align.txt`, which is identical to the C version's demo output.
- On Linux: everything runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.
