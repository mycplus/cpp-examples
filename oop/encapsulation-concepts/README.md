# Encapsulation in object-oriented programming: C++, Java and Python

Source code for the MYCPLUS article
[Encapsulation in Object-Oriented Programming: Private State and Invariants in C++, Java and Python](https://www.mycplus.com/computer-science/oop/encapsulation/).

| File | What it shows |
| --- | --- |
| `cpp/account.cpp`, `java/Account.java`, `python/account.py` | A bank account whose balance changes only through `deposit` and `withdraw`, which reject amounts that would break its rules; identical output |
| `cpp/leak.cpp`, `java/Leak.java`, `python/leak.py` | Returning a private list lets callers break a three-member limit; returning a read-only reference, view or copy does not. The last line differs by language on purpose. |
| `python/properties.py` | A property adds validation behind an attribute name, and a leading underscore is only a convention |

## Test

```sh
bash tests/run_tests.sh g++     # or clang++; needs a JDK and Python 3
```
