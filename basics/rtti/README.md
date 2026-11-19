# RTTI in C++

[![RTTI](https://github.com/mycplus/cpp-examples/actions/workflows/rtti.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/rtti.yml)

Companion code for [RTTI in C++](https://www.mycplus.com/programming/cpp/rtti/) on MYCPLUS: `typeid`, `std::type_info`, `std::type_index` and `dynamic_cast`, in C++17.

| File | What it shows |
| --- | --- |
| `src/type_name.h` | A readable class name from `type_info::name()` on GCC, Clang and MSVC |
| `src/typeid_basics.cpp` | `typeid` on polymorphic and non-polymorphic classes, `std::bad_typeid`, counting objects by type with `std::type_index` |
| `src/dynamic_cast_basics.cpp` | Downcasts that succeed and fail, `std::bad_cast`, a cross-cast and `dynamic_cast<void*>` |
| `src/construction.cpp` | The dynamic type of an object during its constructors and destructors |
| `src/alternatives.cpp` | One task three ways: `dynamic_cast`, a virtual function and `std::variant` |
| `bench/cast_cost.cpp` | Rough cost of a virtual call, `typeid` and `dynamic_cast` (built, not tested) |
| `tests/compile_fail/` | `dynamic_cast` on a non-polymorphic type and from `void*`, which must not compile |
| `run_all.sh`, `run_all.ps1` | Every check in one command for Linux and Windows, including deliberate mistakes a test must catch |

## Build and test

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 on Linux and with MSVC `/W4 /WX /permissive-` on Windows, warnings as errors.
- Each program prints exactly its file in `tests/expected/`, on every compiler (`type_name.h` hides the differences in `type_info::name()`).
- The two programs in `tests/compile_fail/` fail to build with a message naming the problem.
- On Linux: every test also runs under AddressSanitizer and UndefinedBehaviorSanitizer.
