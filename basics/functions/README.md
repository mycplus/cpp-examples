# Functions in C++ examples

Source code for the MYCPLUS article
[Functions in C++: Declarations, Parameters, Overloading and Modern Techniques](https://www.mycplus.com/programming/cpp/functions-in-cpp/).

| File | What it shows |
| --- | --- |
| `src/declare_define.cpp` | A declaration lets `main()` call a function defined later in the file |
| `src/passing.cpp` | Copies made by passing by value, `const` reference, reference and pointer, and by returning by value |
| `src/defaults.cpp` | Default arguments, declared once |
| `src/overloading.cpp` | Overload resolution, including promotions from `char`, `float` and `bool` |
| `src/eval_order.cpp` | Unspecified argument evaluation order: GCC and Clang print different results |
| `src/variadic.cpp` | `std::initializer_list`, a variadic template and a fold expression |
| `src/returning.cpp` | Returning several values with a struct and structured bindings, and a missing value with `std::optional` |
| `src/function_values.cpp` | A function pointer, a lambda and `std::function` passed to other functions |
| `src/constexpr_fn.cpp` | One `constexpr` function evaluated at compile time and at run time |
| `src/dangling_return.cpp` | Returning a reference to a local variable: a warning at compile time, undefined behavior at run time |
| `tests/compile-fail/*.cpp` | Six programs that must not compile, each for a stated reason |

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
bash tests/run_tests.sh g++ c++17     # or clang++, c++20
bash tests/sanitizers.sh g++          # or clang++
```

`run_tests.sh` builds every example with `-Wall -Wextra -pedantic -Werror`, runs it and compares its
output with the article (with a separate expected file per compiler family for `eval_order`), checks
that `dangling_return.cpp` still produces its warning, and checks that each compile-fail example is
rejected with the expected diagnostic. `sanitizers.sh` runs every correct example under
AddressSanitizer and UndefinedBehaviorSanitizer and expects no report, and expects AddressSanitizer
to report `dangling_return`. The macOS job runs `run_tests.sh` with Apple Clang; the Windows job builds
with MSVC at `/W4 /WX` and compares the outputs, except `eval_order`, whose output it prints.
