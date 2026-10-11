# Object-oriented programming overview: C++, Java and Python

Source code for the MYCPLUS article
[Object-Oriented Programming: Concepts, Pillars and Techniques in C++, Java and Python](https://www.mycplus.com/computer-science/oop/object-oriented-programming/).

| File | What it shows |
| --- | --- |
| `python/shipping_procedural.py` | The procedural version: one function chooses a shipping cost with `if` statements |
| `cpp/shipping.cpp`, `java/Shipping.java`, `python/shipping.py` | The object-oriented version: an abstract shipping method, two objects of one class, two more classes, and one function that works with all of them; identical output |

## Test

```sh
bash tests/run_tests.sh g++     # or clang++; needs a JDK and Python 3
```
