# JSON Validation in C++ (nlohmann/json)

[![JSON Validation](https://github.com/mycplus/cpp-examples/actions/workflows/json-validation.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/json-validation.yml)

Companion code for [Valid JSON: Rules, Common Errors and How to Validate It](https://www.mycplus.com/programming/how-to-validate-json/) on MYCPLUS.

| File | What it does |
| --- | --- |
| `src/validate_json.cpp` | Reports whether each file is valid JSON, with nlohmann/json's error message and position |
| `src/validate_user.cpp` | Checks that a user record has the required fields and types (the rules of `cases/schema/user.schema.json`) |
| `src/edge_cases.cpp` | Shows what nlohmann/json does with duplicate keys, big integers, a BOM, a lone surrogate, NaN and deep nesting |
| `cases/` | The test files, shared with the JavaScript and Python examples |
| `tests/expected/` | Expected output; `verdicts.txt` and `user-verdicts.txt` are identical in all three languages |
| `run_all.sh` | GCC, Clang, ASan and UBSan, plus three deliberate mistakes the tests must catch |
| `run_all.ps1` | The same checks with MSVC on Windows |

Build and test (CMake 3.20 or later downloads nlohmann/json 3.12.0):

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The same cases in other languages: [JavaScript](https://github.com/mycplus/javascript-examples/tree/main/data-formats/json-validation) and [Python](https://github.com/mycplus/python-examples/tree/main/data-formats/json-validation).
