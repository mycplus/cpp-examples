# Direct3D 12: create a device, check DXR, create a command queue

[![d3d12-device-info](https://github.com/mycplus/cpp-examples/actions/workflows/d3d12-device-info.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/d3d12-device-info.yml)

Companion code for [Graphics Technologies in AAA Game Development with C++](https://www.mycplus.com/game-development/incorporating-graphics-technologies-in-aaa-game-development-using-cpp/) on MYCPLUS. It is the Direct3D 12 counterpart of [vulkan-device-info](../vulkan-device-info/): enumerate the GPUs with DXGI, create an `ID3D12Device`, ask which **DirectX Raytracing (DXR)** tier the driver supports, and create the direct command queue that a renderer submits its command lists to. With no hardware GPU it falls back to **WARP**, Microsoft's software rasterizer.

| File | What it does |
| --- | --- |
| `src/d3d12_device_info.cpp` | DXGI adapter selection (high-performance first), device creation, DXR tier check, command queue. Requires Windows 10 or later. |

## Build

Visual Studio 2022 or Build Tools, from a developer prompt:

```sh
cmake -S . -B build
cmake --build build --config Release
build\Release\d3d12_device_info.exe
```

MinGW-w64 (including cross-compiling from Linux):

```sh
x86_64-w64-mingw32-g++ -std=c++17 -Wall -Wextra -O2 -o d3d12_device_info.exe src/d3d12_device_info.cpp -ld3d12 -ldxgi
```

## What the build checks

On `windows-latest`, the `d3d12-device-info` workflow builds the program with MSVC at `/W4 /WX` and runs it; it must print `PASS`. GitHub's runners have no GPU, so this run uses WARP. A Linux job cross-compiles it with MinGW-w64 and `-Wall -Wextra -Werror`.
