# A small CPU ray tracer in C++

[![cpu-ray-tracer](https://github.com/mycplus/cpp-examples/actions/workflows/cpu-ray-tracer.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/cpu-ray-tracer.yml)

Companion code for [Graphics Technologies in AAA Game Development with C++](https://www.mycplus.com/game-development/incorporating-graphics-technologies-in-aaa-game-development-using-cpp/) on MYCPLUS. It shows what a ray tracer does, without a GPU: for every pixel it fires a ray from the camera, finds the nearest surface, fires a **shadow ray** towards the light and, for shiny surfaces, a **reflection ray**. The result is linear HDR colour, which is tone mapped (Reinhard) and gamma encoded before it is written to a PPM image.

| File | What it is |
| --- | --- |
| `src/raytracer.hpp` | Header-only tracer: vectors, ray-sphere intersection, shadows, reflections, a pinhole camera and tone mapping |
| `src/ray_tracer.cpp` | Renders the demo scene (three spheres on a ground sphere) to a binary PPM and prints a summary |
| `tests/` | Unit tests and the expected summary for a 160x90 render |

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Render a full-size image (any viewer that opens PPM, such as GIMP or IrfanView, will show it):

```sh
./build/ray_tracer render.ppm 1280 720
```

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 and with MSVC, warnings as errors.
- Ray-sphere intersection: head-on hits, misses, rays pointing away, rays starting inside a sphere and tangent rays.
- The tracer returns the nearest of two spheres, with the correct normal.
- Shadow rays: the top of a sphere is lit, the ground behind it (away from the light) is in shadow.
- A miss returns the sky colour, and a perfect mirror facing the camera reflects the sky behind it.
- Tone mapping keeps black at 0, keeps order, and only very bright values reach 255.
- A 160x90 render prints exactly `tests/expected/render_160x90.txt` (pixel counts per object and in shadow).
- Tests and demo run clean under AddressSanitizer and UndefinedBehaviorSanitizer.
