# Object-oriented programming in C++ examples

Source code for the MYCPLUS article
[Object-Oriented Programming in C++: Classes, Inheritance, Polymorphism and Encapsulation](https://www.mycplus.com/programming/cpp/object-orientation/).

| File | What it shows |
| --- | --- |
| `src/library.cpp` | Abstraction, encapsulation, inheritance and polymorphism in one program: an abstract `Document`, `EBook` and `AudioBook`, owned through `std::unique_ptr` |
| `src/missing_override.cpp` | A function that looks like an override but is not, because it lacks `const` |
| `src/slicing.cpp` | Passing or storing a derived object by value keeps only the base part |
| `src/operators.cpp` | Operator overloading for a `Money` type, and why overloading `&&` loses short-circuit evaluation |
| `src/variant_shapes.cpp` | Run-time dispatch over a closed set of types with `std::variant` and `std::visit`, no base class |
| `tests/compile-fail/override_typo.cpp` | Must not compile: `override` on a function that overrides nothing |
| `tests/compile-fail/default_private.cpp` | Must not compile: class members are private by default |

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
bash tests/run_tests.sh g++ c++17     # or clang++, c++20
bash tests/sanitizers.sh g++          # or clang++
```

`run_tests.sh` builds every example with `-Wall -Wextra -pedantic -Werror`, runs it and compares its
output with the article, checks that `missing_override.cpp` still produces `-Woverloaded-virtual`, and
checks that both compile-fail examples are rejected. `sanitizers.sh` runs every example under
AddressSanitizer and UndefinedBehaviorSanitizer and expects no report. The macOS and Windows jobs
build with warnings as errors and compare the same outputs.
