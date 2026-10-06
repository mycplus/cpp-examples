# Direct3D 11: window, flip-model swap chain, clear and present

Companion code for the MYCPLUS article
[DirectX Components Explained](https://www.mycplus.com/game-development/directx-components/).
It is the modern counterpart of a classic DirectDraw sample: a window, a front
and a back buffer, a per-frame clear, and a present.

| File | What it does |
| --- | --- |
| `src/d3d11_clear.cpp` | Win32 window, Direct3D 11 device, `DXGI_SWAP_EFFECT_FLIP_DISCARD` swap chain with two buffers, resize handling, device-removed check. Requires Windows 10 or later. |
| `src/warp_clear_test.cpp` | Headless test: clears a 4x4 BGRA texture to green on the WARP software rasterizer and checks the bytes read back. |

## Build

Visual Studio 2022 or Build Tools, from a developer prompt:

```sh
cmake -S . -B build
cmake --build build --config Release
build\Release\warp_clear_test.exe
```

MinGW-w64 (including cross-compiling from Linux):

```sh
x86_64-w64-mingw32-g++ -std=c++17 -Wall -Wextra -O2 -mwindows -o d3d11_clear.exe src/d3d11_clear.cpp -ld3d11
```

## What the build checks

On `windows-latest`, the `d3d11-clear` workflow builds both programs with MSVC
at `/W4 /WX` and runs `warp_clear_test`, which must print
`first pixel bytes: 00 FF 00 FF` and `PASS`. A Linux job cross-compiles both with
MinGW-w64 and `-Wall -Wextra -Werror`.

No job opens a window: the windowed program is built, not run, in CI.
