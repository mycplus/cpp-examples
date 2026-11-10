#!/usr/bin/env bash
# run_all.sh - every check behind the article's "What I Verified" table for
# the C++ version, on Ubuntu or WSL. Run from this directory:  bash run_all.sh
set -u
cd "$(dirname "$0")"
results=()
row() { results+=("$(printf '%-44s %s' "$1" "$2")"); }
run() { local name=$1; shift; if "$@" >/tmp/lcs_cpp_run_all.log 2>&1; then row "$name" PASS; else row "$name" FAIL; cat /tmp/lcs_cpp_run_all.log; fi; }

if ! command -v cmake >/dev/null; then echo "cmake is required (sudo apt install cmake)"; exit 2; fi
for cxx in g++ clang++; do
  if ! command -v "$cxx" >/dev/null; then row "Build and CTest, $cxx" "SKIP (not installed)"; continue; fi
  for std in 17 20; do
    run "Build and CTest, $cxx C++$std" bash -c "rm -rf build-$cxx-$std && cmake -S . -B build-$cxx-$std -DCMAKE_CXX_COMPILER=$cxx -DCMAKE_CXX_STANDARD=$std >/dev/null && cmake --build build-$cxx-$std && ctest --test-dir build-$cxx-$std --output-on-failure"
  done
done
run "AddressSanitizer + UBSan (g++), CTest" bash -c "rm -rf build-asan && cmake -S . -B build-asan -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-sanitize-recover=all -g' >/dev/null && cmake --build build-asan && ctest --test-dir build-asan --output-on-failure"
if command -v valgrind >/dev/null; then
  run "Valgrind, test suite" valgrind -q --error-exitcode=9 --leak-check=full build-g++-17/test_lcs
else row "Valgrind, test suite" "SKIP (not installed)"; fi

echo
echo "Environment: $(uname -sr), $(grep -s PRETTY_NAME /etc/os-release | cut -d'"' -f2)"
grep -qi microsoft /proc/version 2>/dev/null && echo "Running under WSL"
g++ --version 2>/dev/null | head -1
clang++ --version 2>/dev/null | head -1
printf '%s\n' "${results[@]}"
printf '%s\n' "${results[@]}" | grep -q FAIL && exit 1 || exit 0
