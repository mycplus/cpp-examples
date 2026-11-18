# Named Pipes in C++

[![Named Pipes](https://github.com/mycplus/cpp-examples/actions/workflows/named-pipes.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/named-pipes.yml)

Companion code for [Named Pipes in C++](https://www.mycplus.com/programming/cpp/named-pipes/) on MYCPLUS: a Windows named pipe client and three servers, in C++17 with the Win32 API.

| File | What it shows |
| --- | --- |
| `src/pipe_common.h` | A move-only RAII handle, error codes by name, and whole-message reads and writes |
| `src/pipe_server.cpp` | One pipe instance, one client at a time |
| `src/pipe_client.cpp` | Connects (waiting while the server is busy), switches to message mode, sends each argument |
| `src/thread_server.cpp` | One pipe instance and one `std::thread` per client |
| `src/overlapped_server.cpp` | Four instances served by one thread with overlapped I/O and `WaitForMultipleObjects` |
| `tests/pipe_test.cpp` | Starts servers and clients together and prints a transcript for CTest to compare |
| `run_all.ps1` | MSVC builds (C++17 and C++20), every test, and six deliberate mistakes that a test must catch |
| `run_all.sh` | MinGW-w64 cross-compile from Linux, warnings as errors (the programs run only on Windows) |

## Build and test (Windows)

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Try it by hand

In one terminal:

```sh
build\bin\pipe_server.exe demo 2
```

In another:

```sh
build\bin\pipe_client.exe demo hello "named pipes"
```

## What the tests check

Seven scenarios, each comparing a transcript with `tests/expected/`:

- `simple`: two clients, one after the other, on the one-instance server.
- `thread` and `overlapped`: three clients at the same time.
- `long-simple` and `long-overlapped`: a 1,500-byte message, longer than every read buffer.
- `duplicate-name`: a second server cannot take a pipe name that is in use.
- `no-server`: the client gives up after five seconds with a clear message.

When clients run at the same time, the servers number them in whatever order they arrive, so those transcripts mask the numbers and sort the server's lines.
