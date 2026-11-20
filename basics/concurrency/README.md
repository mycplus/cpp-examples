# Concurrency in C++

[![Concurrency](https://github.com/mycplus/cpp-examples/actions/workflows/concurrency.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/concurrency.yml)

Companion code for [Concurrency in C++](https://www.mycplus.com/programming/cpp/concurrency/) on MYCPLUS: threads, mutexes, atomics, condition variables, futures and the C++20 synchronization primitives.

| File | What it shows |
| --- | --- |
| `src/threads_basics.cpp` | Starting and joining `std::thread`, passing arguments, `std::ref` |
| `src/counter.cpp` | One counter, four threads: with a `std::mutex`, with `std::atomic`, and racy (for ThreadSanitizer) |
| `src/bounded_queue.h` | A thread-safe bounded queue with `std::condition_variable` and `close()` |
| `src/producer_consumer.cpp` | Two producers and three consumers sharing that queue |
| `src/futures.cpp` | `std::async`, exceptions through futures, `std::promise`, `std::packaged_task` |
| `src/transfers.cpp` | Locking two mutexes with `std::scoped_lock` to avoid deadlock |
| `src/cpp20_sync.cpp` | C++20: `std::jthread` with a stop token, `std::latch`, `std::barrier`, `std::counting_semaphore` |
| `run_all.sh`, `run_all.ps1` | Every check in one command for Linux and Windows, including deliberate mistakes a test must catch |

## Build and test

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

`cpp20_sync` is always built as C++20; everything else follows `CMAKE_CXX_STANDARD` (17 by default).

## What the build checks

- Compiles with g++ and clang++ under C++17 and C++20 on Linux and with MSVC `/W4 /WX /permissive-` on Windows, warnings as errors.
- Each program prints exactly its file in `tests/expected/`. Every test has a 60-second timeout, because a concurrency bug often shows up as a hang.
- On Linux: every test also runs under AddressSanitizer + UndefinedBehaviorSanitizer and under ThreadSanitizer, and ThreadSanitizer must report the race in `counter racy`.
