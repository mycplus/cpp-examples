# Inheritance in C++ examples

Source code for the MYCPLUS article
[Inheritance in C++: Access, Constructors, Overriding and the Pitfalls That Compile](https://www.mycplus.com/programming/cpp/inheritance-in-cpp/).

| File | What it shows |
| --- | --- |
| `src/basics.cpp` | A derived class that reuses its base's members, constructor call, and a function that accepts both |
| `src/access.cpp` | What a derived class can reach; `struct` vs `class` default inheritance |
| `src/order.cpp` | Construction and destruction order of bases and members |
| `src/inheriting_ctors.cpp` | `using Base::Base;` (C++11) |
| `src/hiding.cpp` | Name hiding, the scope operator, and `using Base::f;` |
| `src/final_and_override.cpp` | Virtual functions, `override` and `final` |
| `src/array_pitfall.cpp` | Walking an array of derived objects through a base pointer (undefined behavior) |
| `src/virtual_dtor.cpp` | Deleting through a base pointer with and without a virtual destructor; the broken case runs only with an argument |
| `tests/compile-fail/*.cpp` | Five programs that must not compile, each for a stated reason |

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
bash tests/run_tests.sh g++ c++17     # or clang++, c++20
bash tests/sanitizers.sh g++          # or clang++
```
