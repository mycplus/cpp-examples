# Abstraction in object-oriented programming: C++, Java and Python

Source code for the MYCPLUS article
[Abstraction in Object-Oriented Programming: Abstract Classes and Interfaces in C++, Java and Python](https://www.mycplus.com/computer-science/oop/abstraction/).

| File | What it shows |
| --- | --- |
| `cpp/greeting.cpp`, `java/Greeting.java`, `python/greeting.py` | A `greeting()` function written against an abstract `Clock`, run with a fixed clock and the system clock; identical output |
| `python/protocol.py` | A `typing.Protocol` matched by a class with no base class, and an abstract base class that refuses to create an incomplete subclass |

## Test

```sh
bash tests/run_tests.sh g++     # or clang++; needs a JDK and Python 3
```
