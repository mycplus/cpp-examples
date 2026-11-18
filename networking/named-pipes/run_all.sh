#!/bin/sh
# run_all.sh - checks that can run on Linux: every program cross-compiled for
# Windows with MinGW-w64 in C++17 and C++20, with warnings as errors.
# The programs themselves run only on Windows; use run_all.ps1 there.
# Needs: x86_64-w64-mingw32-g++ (Debian/Ubuntu package g++-mingw-w64-x86-64-posix).
# Run from this directory:  sh run_all.sh
set -u
cd "$(dirname "$0")"
CXX=${CXX:-x86_64-w64-mingw32-g++}
out=$(mktemp -d)
status=0
for std in 17 20; do
  for f in src/pipe_server.cpp src/pipe_client.cpp src/thread_server.cpp \
           src/overlapped_server.cpp tests/pipe_test.cpp; do
    if "$CXX" -std=c++$std -DUNICODE -D_UNICODE -Wall -Wextra -pedantic -Werror -Wconversion \
         -O2 -o "$out/$(basename "$f" .cpp).exe" "$f"; then
      printf '%-50s PASS\n' "MinGW-w64 C++$std $f"
    else
      printf '%-50s FAIL\n' "MinGW-w64 C++$std $f"
      status=1
    fi
  done
done
rm -rf "$out"
exit $status
