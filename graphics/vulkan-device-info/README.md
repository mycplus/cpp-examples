# Vulkan from C++: create an instance and list the GPUs

[![vulkan-device-info](https://github.com/mycplus/cpp-examples/actions/workflows/vulkan-device-info.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/vulkan-device-info.yml)

Companion code for [Graphics Technologies in AAA Game Development with C++](https://www.mycplus.com/game-development/incorporating-graphics-technologies-in-aaa-game-development-using-cpp/) on MYCPLUS. It is the first step of every Vulkan program: create a `VkInstance`, enumerate the physical devices (GPUs), and look at what each one offers: its type, Vulkan version, queue families, and whether it supports hardware ray tracing (`VK_KHR_ray_tracing_pipeline` and `VK_KHR_acceleration_structure`).

| File | What it is |
| --- | --- |
| `src/vulkan_device_info.cpp` | Instance creation, device enumeration with Vulkan's two-call pattern, queue families and the ray tracing check |

## Build and test

You need the Vulkan headers and loader (the [Vulkan SDK](https://vulkan.lunarg.com/) on Windows and macOS, or `libvulkan-dev` on Ubuntu) and at least one Vulkan driver.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

No GPU? Mesa's **lavapipe** is a Vulkan driver that runs on the CPU (`mesa-vulkan-drivers` on Ubuntu). Force it with:

```sh
VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/lvp_icd.json ./build/vulkan_device_info
```

Sample output on lavapipe (Mesa 25.2, Ubuntu 24.04):

```text
Vulkan devices: 1

llvmpipe (LLVM 20.1.2, 256 bits) (CPU (software))
  Vulkan API 1.4.318
  max 2D texture: 16384 x 16384
  queue family 0: 1 queue(s) graphics compute transfer
  ray tracing pipeline: yes
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20, warnings as errors, against the system Vulkan headers.
- On lavapipe, the program creates an instance, finds at least one device, and reports a queue family with graphics and compute.
- It runs clean under AddressSanitizer (including leak checking) and UndefinedBehaviorSanitizer.

There is no Windows job: GitHub's Windows runners have no Vulkan SDK or driver installed by default.
