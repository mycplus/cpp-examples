# Classes in object-oriented programming: C++, Java and Python

Source code for the MYCPLUS article
[Classes in Object-Oriented Programming: Members, Constructors and Static Members in C++, Java and Python](https://www.mycplus.com/computer-science/oop/classes/).

| File | What it shows |
| --- | --- |
| `cpp/temperature.cpp`, `java/Temperature.java`, `python/temperature.py` | One class in three languages: private state, a validating constructor, a computed value, a static factory and a per-class counter; identical output |
| `python/class_attribute_pitfall.py` | A mutable class attribute shared by every object, and the fix |
| `python/name_mangling.py` | Python's underscore conventions and double-underscore name mangling |

## Test

```sh
bash tests/run_tests.sh g++     # or clang++; needs a JDK and Python 3
```
