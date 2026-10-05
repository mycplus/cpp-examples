# Smart Pointers in C++

[![smart-pointers](https://github.com/mycplus/cpp-examples/actions/workflows/smart-pointers.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/smart-pointers.yml)

Source code for the MYCPLUS article
[Smart Pointers in C++: unique_ptr, shared_ptr and weak_ptr](https://www.mycplus.com/programming/cpp/smart-pointers-in-modern-cpp/).

Each program in `src/` is self-contained and matches a listing in the article.

| File | What it shows |
| --- | --- |
| `src/raw_vs_unique.cpp` | An early return that leaks with `new`/`delete` and does not with `std::unique_ptr` |
| `src/unique_basics.cpp` | `make_unique`, moving, sink and factory functions, `reset`, `release`, the array form |
| `src/sizes.cpp` | `sizeof` for each smart pointer and for three kinds of custom deleter |
| `src/file_handle.cpp` | `std::unique_ptr<FILE, FileCloser>` closing a C file handle |
| `src/allocations.cpp` | Heap allocations made by `make_shared` and by `shared_ptr<T>(new T)` |
| `src/shared_basics.cpp` | How `use_count()` changes with copies, moves, scopes and `reset()` |
| `src/cycle_leak.cpp` | Two objects owning each other through `shared_ptr`; neither is destroyed |
| `src/cycle_fixed.cpp` | The same pair with a `weak_ptr` back-pointer |
| `src/shared_from_this.cpp` | `enable_shared_from_this`, and `std::bad_weak_ptr` when it is misused |
| `src/long_list.cpp` | Recursive destruction of a long `unique_ptr` chain, and the iterative fix |
| `src/threads.cpp` | Copying a `shared_ptr` across threads (safe) vs assigning one object from two threads (a race) |
| `src/atomic_shared.cpp` | C++20 `std::atomic<std::shared_ptr<T>>` where the library provides it, a mutex otherwise |
| `tests/compile-fail/copy_unique.cpp` | Must not compile: `unique_ptr` has no copy constructor |
| `tests/sanitizer-fail/two_owners.cpp` | Two control blocks for one object; AddressSanitizer reports a double free |

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
bash tests/run_tests.sh "$PWD/build"      # add --portable on macOS or Windows
bash tests/sanitizers.sh g++              # Linux only; also accepts clang++
```

C++17 throughout, except `atomic_shared.cpp`, which is built as C++20. It uses
`std::atomic<std::shared_ptr<T>>` when the standard library defines
`__cpp_lib_atomic_shared_ptr` (libstdc++ does) and falls back to a
`std::mutex` when it does not (libc++ 18, and the libc++ in Xcode 26.6, do not). Warnings are
errors (`-Wall -Wextra -pedantic -Werror`, or `/W4 /WX` on MSVC).

## What the build checks

- **Linux (GCC and Clang):** builds every example, runs it, and compares its
  output with `tests/expected/`. The allocation counts and byte sizes in
  `allocations.txt` are libstdc++ figures and are compared on Linux only.
- **Sanitizers (Linux, GCC and Clang):** the correct examples produce no
  AddressSanitizer or UndefinedBehaviorSanitizer report; the raw-pointer leak,
  the double free and the recursive-destruction stack overflow are each
  reported; ThreadSanitizer reports the shared-assignment race and nothing for
  the copies or the `std::atomic<std::shared_ptr>` version; copying a
  `unique_ptr` fails to compile.
- **Linux (Clang with libc++), macOS (Apple Clang) and Windows (MSVC):** build
  with warnings as errors and compare the outputs that do not depend on the
  standard library implementation. Output comparisons ignore `\r`, because MSVC
  writes `\r\n` to stdout.

The `shared_ptr` cycle is checked by its output (no destructor runs), not by
LeakSanitizer, whose result for that program varies with the compiler and
flags. The node counts at which `long_list` overflows the stack, the byte sizes
and the allocation sizes are not checked by any job: they depend on the
platform and were measured on one machine for the article.

## Licence

See the repository root.
