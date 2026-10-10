# Multiple inheritance in C++ examples

Source code for the MYCPLUS article
[Multiple Inheritance in C++: Interfaces, Name Clashes, the Diamond Problem and Virtual Bases](https://www.mycplus.com/programming/cpp/multiple-inheritance/).

| File | What it shows |
| --- | --- |
| `src/interfaces.cpp` | One class implementing two abstract interfaces; each base part sits at its own offset |
| `src/name_clash.cpp` | The same member name in two bases, resolved with a using-declaration and qualification |
| `src/diamond.cpp` | The diamond without virtual inheritance: two `Device` subobjects |
| `src/virtual_base.cpp` | The diamond with virtual inheritance: one `Device`, constructed by the most-derived class |
| `src/base_order.cpp` | Bases are constructed in declaration order; the `-Wreorder` warning |
| `tests/compile-fail/*.cpp` | An ambiguous member and an ambiguous base conversion, both rejected |

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
bash tests/run_tests.sh g++ c++17     # or clang++, c++20
bash tests/sanitizers.sh g++          # or clang++
```
