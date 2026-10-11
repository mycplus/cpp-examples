# Objects in object-oriented programming: C++, Java and Python

Source code for the MYCPLUS article
[Objects in Object-Oriented Programming: Identity, State, Copying and Lifetime in C++, Java and Python](https://www.mycplus.com/computer-science/oop/objects/).

| File | What it shows |
| --- | --- |
| `cpp/identity.cpp`, `java/Identity.java`, `python/identity.py` | Equality versus identity, and what assignment does: C++ copies the object, Java and Python share it. The outputs differ on purpose. |
| `cpp/lifetime.cpp`, `java/Lifetime.java`, `python/lifetime.py` | Deterministic cleanup: a destructor (C++), try-with-resources (Java) and a `with` statement (Python); identical output |
| `python/shallow_copy.py` | `copy.copy` shares nested objects; `copy.deepcopy` does not |

## Test

```sh
bash tests/run_tests.sh g++     # or clang++; needs a JDK and Python 3
```
