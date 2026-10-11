# Polymorphism in object-oriented programming: C++, Java and Python

Source code for the MYCPLUS article
[Polymorphism in C++, Java and Python](https://www.mycplus.com/computer-science/oop/polymorphism/).

| File | What it shows |
| --- | --- |
| `cpp/shapes.cpp`, `java/Shapes.java`, `python/shapes.py` | Runtime polymorphism: one loop calls `area()` on a circle and a rectangle through the base type; identical output |
| `cpp/overloading.cpp`, `java/Overloading.java` | Compile-time polymorphism: three `add()` overloads chosen by argument type |
| `python/duck_typing.py` | Polymorphism without inheritance: three unrelated classes with a `speak()` method |

## Test

```sh
bash tests/run_tests.sh g++     # or clang++; needs a JDK and Python 3
```
