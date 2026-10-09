# C++ memory management examples

Source code for the MYCPLUS article
[C++ Memory Management: new and delete, RAII and the Memory Errors That Compile](https://www.mycplus.com/programming/cpp/memory-management/).

| File | What it shows | Expected result |
| --- | --- | --- |
| `src/new_delete.cpp` | The four forms of `new`/`delete`, value-initialization, `std::bad_alloc` and `new (std::nothrow)` | Runs, output in `tests/expected/` |
| `src/ownership.cpp` | A buffer-owning class written with the rule of five, and the same idea with the rule of zero | Runs, output in `tests/expected/` |
| `src/exception_safety.cpp` | A constructor that throws: raw-pointer members leak, `std::unique_ptr` members do not | Runs; LeakSanitizer reports 32 bytes |
| `src/throwing_destructor.cpp` | Destructors are `noexcept` by default since C++11 | Default mode calls `std::terminate`; `loose` mode catches |
| `src/shallow_copy.cpp` | A class with a destructor and a compiler-generated copy constructor | **Broken on purpose**: double free |
| `src/dangling_temporary.cpp` | Returning a `const&` parameter that refers to a temporary | **Broken on purpose**: dangling reference |
| `src/memory_errors.cpp` | Eight memory errors, one per command-line mode | **Broken on purpose**: run under a sanitizer or Valgrind |

`memory_errors` modes: `leak`, `use-after-free`, `double-free`, `mismatched-delete`,
`heap-overflow`, `return-local`, `invalidated-pointer`, `delete-non-heap`.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/memory_errors use-after-free
```

With AddressSanitizer (GCC or Clang):

```sh
g++ -std=c++17 -g -O0 -fsanitize=address -o memory_errors src/memory_errors.cpp
./memory_errors use-after-free
```

With Valgrind (build without sanitizers):

```sh
g++ -std=c++17 -g -O0 -o memory_errors src/memory_errors.cpp
valgrind --leak-check=full ./memory_errors leak
```

## Tests

```sh
bash tests/run_tests.sh g++ c++17        # or clang++, c++20
bash tests/sanitizers.sh g++             # or clang++
```

`run_tests.sh` builds the correct examples with `-Wall -Wextra -pedantic -Werror`, runs them and
compares their output with the article; it also checks the compiler warnings the article quotes and
that the broken programs fail. `sanitizers.sh` checks that the correct examples run clean under
AddressSanitizer and UndefinedBehaviorSanitizer and that each deliberate bug is reported.
The allocation-failure example runs under UBSan only, because ASan's allocator aborts on
oversized requests instead of throwing `std::bad_alloc`.

The macOS and Windows jobs build the four portable examples with warnings as errors and compare
their output; the broken programs are Linux-only, because their behavior is undefined and the
article reports what happened on Linux.
