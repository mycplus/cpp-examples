# Ten C++ Libraries, One Program Each

[![Ten C++ Libraries](https://github.com/mycplus/cpp-examples/actions/workflows/ten-cpp-libraries.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/ten-cpp-libraries.yml)

Companion code for [10 C++ Libraries Every Student Should Know, Each With a Tested Example](https://www.mycplus.com/programming/cpp/10-cpp-libraries-that-every-student-should-know/) on MYCPLUS.

| Program | Library | What it does |
| --- | --- | --- |
| `src/stl_ranges.cpp` | C++ standard library | Counts words with `std::map` and ranks them with `std::ranges` (C++20) |
| `src/boost_multiprecision.cpp` | Boost.Multiprecision | Computes 30! and 2^128 exactly |
| `src/qt_widgets.cpp` | Qt 6 Widgets | A window whose button signal updates a label; saves a PNG of itself |
| `src/eigen_solve.cpp` | Eigen | Solves a 3×3 system and a least-squares line fit |
| `src/curl_fetch.cpp` | libcurl | Reads a `file://` URL with the easy interface and reports a failed transfer |
| `src/tbb_parallel.cpp` | oneTBB | `parallel_reduce` and `parallel_sort`, checked against the standard library; `--time` prints timings |
| `src/opencv_edges.cpp` | OpenCV | Canny edge detection and contour counting on a drawn image |
| `src/catch2_tests.cpp` | Catch2 v3 | Unit tests for a small parsing function |
| `src/poco_json.cpp` | POCO | Parses JSON and splits a URL |
| `src/fmt_format.cpp` | {fmt} | Formatting with `fmt`, compared with C++20 `std::format` |

## Build and test (Ubuntu 24.04)

```sh
sudo apt-get install libboost-dev libfmt-dev catch2 libeigen3-dev libcurl4-openssl-dev \
                     libtbb-dev libopencv-dev libpoco-dev qt6-base-dev
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Versions tested: Boost 1.83, {fmt} 9.1, Catch2 3.4, Eigen 3.4, libcurl 8.5, oneTBB 2021.11, OpenCV 4.6, POCO 1.11, Qt 6.4, with GCC 13.3 and Clang 18.1.3 under C++20. On Windows and macOS, vcpkg or Homebrew provide the same packages; the CI does not test those platforms.

## What the build checks

- Every program compiles with g++ and clang++ under C++20 with `-Wall -Wextra -pedantic -Werror`.
- Each program prints exactly the output in `tests/expected/`; the Qt program runs with the `offscreen` platform plugin.
- The Catch2 tests pass.
- Everything runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.
- Timings from `tbb_parallel --time` are not checked: they depend on the machine.
