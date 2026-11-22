# Which Programs Need VCRUNTIME140_1.dll?

[![VCRUNTIME](https://github.com/mycplus/cpp-examples/actions/workflows/vcruntime.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/vcruntime.yml)

Companion code for [VCRUNTIME140_1.dll Was Not Found: How to Fix It](https://www.mycplus.com/featured-articles/how-to-fix-vcruntime140_1-dll-library-errors/) on MYCPLUS. It shows, with tests, which MSVC builds import `VCRUNTIME140_1.dll` and how to build without it.

| File | What it shows |
| --- | --- |
| `src/eh_demo.cpp` | A program that throws and catches exceptions, built four ways |
| `src/hello.cpp` | A program with no exceptions, for comparison |
| `src/runtime_check.cpp` | Reads the Redistributable's registry values (both registry views) and reports which runtime DLLs load, and their file versions |
| `CMakeLists.txt` | The builds, plus tests that run `dumpbin /dependents` and check the imported DLLs per architecture |
| `run_all.ps1` | Builds x64, x86 and ARM64, runs the tests and deliberate mistakes, and records what is installed (read only) |
| `run_all.sh` | MinGW-w64 compile check from Linux |

## Build and test (Windows, Visual Studio 2019 or later)

```sh
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Use `-A Win32` or `-A ARM64` for the other architectures.

## What the tests check (x64)

| Program | Imports `VCRUNTIME140_1.dll` |
| --- | --- |
| `eh_demo` (default `/MD`) | Yes |
| `eh_demo_nofh4` (`/d2FH4-`) | No |
| `eh_demo_static` (`/MT`) | No runtime DLLs at all |
| `hello` (no exceptions) | No |

For x86 builds, no program imports `VCRUNTIME140_1.dll`. All of the above was confirmed with MSVC 14.44 on Windows 11.
