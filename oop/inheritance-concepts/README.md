# Inheritance in object-oriented programming: C++, Java and Python

Source code for the MYCPLUS article
[Inheritance in OOP: Types, Examples in C++, Java and Python, and When Not to Use It](https://www.mycplus.com/tutorials/object-oriented-programming/inheritance/).

| File | What it shows |
| --- | --- |
| `cpp/notifications.cpp`, `java/Notifications.java`, `python/notifications.py` | One base class and two subclasses that override one step; the same output in all three languages |
| `cpp/fragile_base.cpp`, `java/FragileBase.java`, `python/fragile_base.py` | The fragile base class problem: a subclass that double-counts because of how its base is implemented |
| `python/composition.py` | The same counter built by composition, which counts correctly |
| `python/types_of_inheritance.py` | Single, hierarchical, multiple and multilevel inheritance, with Python's method resolution order |

## Test

```sh
bash tests/run_tests.sh g++     # or clang++; needs a JDK and Python 3
```

The C++ and multiple-inheritance details are in the companion guides
[Inheritance in C++](https://www.mycplus.com/programming/cpp/inheritance-in-cpp/) and
[Multiple Inheritance in C++](https://www.mycplus.com/programming/cpp/multiple-inheritance/).
