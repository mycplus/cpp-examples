# MD5, SHA-1 and SHA-256 in C++

[![Hash functions](https://github.com/mycplus/cpp-examples/actions/workflows/hash-functions.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/hash-functions.yml)

Companion code for [MD5, SHA-1 and SHA-256: Hash Functions in C, C++, Java, Python and C#](https://www.mycplus.com/computer-science/algorithms/md5-sha1-sha256-hash-functions/) on MYCPLUS: a header-only C++17 implementation of the three hashes and HMAC, and the same digests from OpenSSL's EVP interface.

> For learning and testing. Never store passwords with a plain hash, and never build your own cryptography for real data: use a maintained library (OpenSSL, the platform's own crypto API) and, for passwords, a slow password hash such as Argon2id, scrypt or bcrypt.

| File | What it is |
| --- | --- |
| `include/digest.hpp` | `Md5`, `Sha1` and `Sha256` classes on one `BlockHash` base, plus `hash<>()`, `hmac<>()`, `digest_equal()` and `to_hex()` |
| `src/digest_demo.cpp` | Hashes three short messages; prints exactly what the C version prints |
| `src/openssl_digest.cpp` | The same digests and an HMAC through OpenSSL's EVP interface, checked against `digest.hpp` |
| `tests/test_vectors.cpp` | Known answers, 306 message lengths, every split offset up to 130 bytes, HMAC-MD5, HMAC-SHA-1 and HMAC-SHA-256 |
| `tests/mutations.py` | Breaks `digest.hpp` 19 ways, one at a time, and confirms the tests notice |

The vectors are in `tests/vectors/`: the RFC 1321 and FIPS 180-4 examples, 306 message lengths generated with Python's `hashlib` (an independent implementation), and the RFC 4231 and RFC 2202 HMAC cases. `tests/vectors/` is identical in all five languages.

## Build and run

OpenSSL is optional; without it, `openssl_digest` is not built.

```sh
sudo apt install libssl-dev    # Debian/Ubuntu, optional
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
python3 tests/mutations.py     # needs g++ or c++
```

## What the build checks

- Compiles with GCC and Clang under C++17 and C++20 on Linux and with MSVC `/W4 /WX` on Windows, warnings as errors.
- Every vector passes; chunked and split hashing agree with one-shot hashing.
- The demo prints the same bytes as the C version's, and the OpenSSL program reports a match for every digest.
- Everything runs clean under AddressSanitizer and UndefinedBehaviorSanitizer.
