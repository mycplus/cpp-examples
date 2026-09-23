# Array Operations (C++)

Access and update, traversal, linear search, insertion, deletion, size and
multi-dimensional arrays, using `std::array` and `std::vector`.

```text
array-operations/
├── CMakeLists.txt
├── include/array_ops.hpp   find_index
├── src/array-demo.cpp      each operation run once
└── tests/test_array_ops.cpp
```

## What the code is careful about

- **`std::array` keeps its length in its type**, so it does not decay to a
  pointer and `size()` is available wherever the object is.
- **`find_index` returns `std::optional<std::size_t>`**, so there is no
  sentinel index — `-1` is not representable in an unsigned type.
- **Insertion and deletion are `std::vector` member functions.** The
  container owns a resizable buffer, so the shifting and any reallocation
  are handled for you. An insertion that reallocates invalidates existing
  iterators, pointers and references into the vector.
- **`operator[]` does not check the index; `at()` throws.** The demo shows
  both.
- **`std::array<std::array<int, C>, R>` is contiguous** and keeps its shape
  in the type, unlike a vector of vectors.

## Build and run

```bash
c++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/array-demo.cpp -o array-demo
c++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/test_array_ops.cpp -o test_array_ops
./array-demo
./test_array_ops
```

Or with CMake:

```bash
cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
```

## Automated builds

GitHub Actions builds with GCC and Clang on Ubuntu at C++17 and C++20,
Apple Clang on macOS and MSVC on Windows, warnings treated as errors, then
runs the test suite and checks the demo reports every operation. Separate
jobs cover the CMake build and a run under AddressSanitizer and
UndefinedBehaviorSanitizer.
