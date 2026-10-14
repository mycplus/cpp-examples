# Serpent Block Cipher in C++

[![Serpent](https://github.com/mycplus/cpp-examples/actions/workflows/serpent.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/serpent.yml)

Companion code for [Serpent Cipher Explained](https://www.mycplus.com/computer-science/algorithms/serpent-cipher/) on MYCPLUS: a header-only C++17 class for the Serpent block cipher (128-bit blocks, keys up to 256 bits), checked against 1,043 published test vectors.

> For learning and testing. This code has not been reviewed for side channels. To protect real data, use a maintained library and an authenticated mode such as AES-GCM or ChaCha20-Poly1305.

| File | What it is |
| --- | --- |
| `include/serpent.hpp` | `mycplus::Serpent`: key schedule, `encrypt` and `decrypt` of one 16-byte block; the inverse S-boxes are computed at compile time |
| `src/serpent_demo.cpp` | Encrypts one block with a 256-bit key and checks two known vectors |
| `tests/test_vectors.cpp` | Runs every vector in `tests/vectors/serpent_nessie.txt` in both directions and checks key-length validation |
| `bench/serpent_speed.cpp` | Throughput in ECB mode over 16 MiB |

The test vectors are the NESSIE set as distributed with [Botan](https://github.com/randombit/botan) (`src/tests/data/block/serpent.vec`). The C version in [mycplus/c-examples](https://github.com/mycplus/c-examples/tree/main/cryptography/serpent) is also compared with libgcrypt.

## Build and run

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/serpent_demo
./build/serpent_speed
```

## What the build checks

- Compiles with GCC and Clang under C++17 and C++20 on Linux and with MSVC `/W4 /WX /permissive-` on Windows, warnings as errors.
- All 1,043 vectors (128-, 192- and 256-bit keys) encrypt and decrypt correctly; keys of 0 and 33 bytes throw `std::invalid_argument`.
- The demo and the vector test print exactly the files in `tests/expected/`.
- Everything runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.
