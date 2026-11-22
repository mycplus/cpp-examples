#!/bin/sh
# run_all.sh - the only check that runs on Linux: every source file
# cross-compiled for Windows with MinGW-w64, warnings as errors. The tests
# themselves need MSVC and dumpbin; run run_all.ps1 on Windows.
# Needs: x86_64-w64-mingw32-g++ (Debian/Ubuntu package g++-mingw-w64-x86-64-posix).
set -u
cd "$(dirname "$0")"
CXX=${CXX:-x86_64-w64-mingw32-g++}
status=0
for std in 17 20; do
  for f in src/*.cpp; do
    if "$CXX" -std=c++$std -Wall -Wextra -pedantic -Werror -Wconversion -c "$f" -o /dev/null; then
      printf '%-50s PASS\n' "MinGW-w64 C++$std $f"
    else
      printf '%-50s FAIL\n' "MinGW-w64 C++$std $f"; status=1
    fi
  done
done
exit $status
