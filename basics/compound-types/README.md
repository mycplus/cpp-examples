# Compound Types in C++

[![Compound Types](https://github.com/mycplus/cpp-examples/actions/workflows/compound-types.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/compound-types.yml)

Companion code for [Compound Types in C++](https://www.mycplus.com/programming/cpp/compound-types/) on MYCPLUS: one short program for each kind of compound type, in C++17.

| File | What it shows |
| --- | --- |
| `src/type_census.cpp` | Which category the compiler reports for one type of each kind, using `<type_traits>` |
| `src/arrays.cpp` | Array-to-pointer decay, a reference to an array, and `std::array` |
| `src/pointers_references.cpp` | Re-pointing a pointer versus assigning through a reference, and `const` placement |
| `src/unions_variant.cpp` | A union, a tagged union, `memcpy` to read a float's bits, and `std::variant` |
| `src/member_pointers.cpp` | One `sort_by` function that sorts by any member, and a pointer to a member function |
| `src/declarations.cpp` | Reading combined declarations such as `int (*)[3]`, checked with `static_assert` |
| `tests/compile_fail/` | An array of references and an unbound reference, which must not compile |
| `pitfalls/inactive_member.cpp` | Deliberately wrong: reads the inactive member of a union (not built) |
| `run_all.sh`, `run_all.ps1` | Every check in one command, for Ubuntu/WSL and for Windows (MSVC) |

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 on Linux and with MSVC `/W4 /WX /permissive-` on Windows, warnings as errors.
- Each program prints exactly its file in `tests/expected/`.
- The two programs in `tests/compile_fail/` fail to build with a message naming the problem; a build that fails for another reason does not pass.
- On Linux: every test runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.

The pointer sizes in the expected output (8 bytes) assume a 64-bit build.
