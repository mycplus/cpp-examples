# Stack in C++

[![stack](https://github.com/mycplus/cpp-examples/actions/workflows/stack.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/stack.yml)

Companion code for [Stack Implementation in C, C++, Java, Python and C#](https://www.mycplus.com/computer-science/data-structures/stack-implementation/) on MYCPLUS.

| File | What it is |
| --- | --- |
| `include/array_stack.hpp` | `ArrayStack<T>`: a stack over `std::vector`'s contiguous storage |
| `include/linked_stack.hpp` | `LinkedStack<T>`: `std::unique_ptr`-owned nodes, destroyed iteratively |
| `src/stack_demo.cpp` | The article's demo, with a bracket checker on `std::stack` |
| `src/linked_demo.cpp` | The same sequence on `LinkedStack<int>` |
| `src/vector_growth.cpp` | Counts `std::vector` reallocations over 1,000,000 `push_back` calls |
| `pitfalls/` | Three deliberately broken programs from the article. **Do not copy them.** |
| `tests/` | Tests against `std::stack`, and the expected output of each demo |

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 with `-Wall -Wextra -pedantic -Werror`, and with MSVC under `/W4 /WX /permissive-`.
- Both stacks agree with `std::stack<std::string>` over 100,000 random operations.
- Both work with a move-only element type (`std::unique_ptr<int>`).
- `pop()` and `top()` on an empty stack throw `std::out_of_range`; `try_pop()` returns `std::nullopt`.
- Destroying, clearing and move-assigning a `LinkedStack` of 1,000,000 nodes does not recurse.
- `stack_demo` and `linked_demo` print exactly the output shown in the article.
- The tests and demos run clean under AddressSanitizer and UndefinedBehaviorSanitizer.
- Each pitfall is caught: ASan reports `stack-overflow` for recursive node destruction, libstdc++ assertions abort `top()` on an empty `std::stack`, and the `NULL` sentinel aborts at run time under C++17 and fails to compile under C++23.

`vector_growth` is built but its figure is not checked: the C++ standard leaves the growth factor to each standard library implementation.
