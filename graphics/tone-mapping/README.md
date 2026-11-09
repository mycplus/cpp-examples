# HDR tone mapping in C++ (Reinhard, extended Reinhard, ACES) and GLSL

[![tone-mapping](https://github.com/mycplus/cpp-examples/actions/workflows/tone-mapping.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/tone-mapping.yml)

Companion code for [Graphics Technologies in AAA Game Development with C++](https://www.mycplus.com/game-development/incorporating-graphics-technologies-in-aaa-game-development-using-cpp/) on MYCPLUS. Games render lighting in **high dynamic range** (HDR): values brighter than 1.0 are normal. A **tone mapping** curve compresses them into the 0 to 1 range a display shows, and the sRGB transfer curve encodes the result for an ordinary (SDR) screen.

| File | What it is |
| --- | --- |
| `src/tonemap.hpp` | Header-only operators: Reinhard, extended Reinhard, Narkowicz's fitted ACES curve, exposure in stops, sRGB encoding |
| `src/tone_mapping.cpp` | Prints a table of how each curve treats the same HDR values |
| `shaders/tonemap.frag` | The same ACES curve as a GLSL fragment shader for a Vulkan post-processing pass |
| `tests/` | Unit tests and expected output |

Narkowicz's original post multiplies the input by 0.6 before the curve to match the reference ACES exposure; here exposure is a separate, explicit step.

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

If `glslangValidator` (from the Vulkan SDK, or the `glslang-tools` package on Ubuntu) is on the path, CTest also compiles the shader to SPIR-V.

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 and with MSVC, warnings as errors.
- Reinhard maps 1 to 0.5 and never reaches 1; extended Reinhard maps the chosen white point to exactly 1.
- The ACES fit maps 0 to 0, clamps large values to 1, and matches its formula at middle grey.
- Every curve is monotonic from 0 to 64.
- Exposure doubles or halves per stop; sRGB encoding matches published reference points (0.18 to about 0.461).
- The demo prints exactly `tests/expected/tone_mapping.txt`.
- On Linux, `shaders/tonemap.frag` compiles to SPIR-V with glslangValidator.
- Tests and demo run clean under AddressSanitizer and UndefinedBehaviorSanitizer.
