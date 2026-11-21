#!/bin/sh
# Build and test everything. Call it as: sh run_all.sh   (the executable bit is lost in zip files)
set -e
for std in 17 20; do
  rm -rf "build-cxx$std"
  cmake -S . -B "build-cxx$std" -DCMAKE_CXX_STANDARD=$std
  cmake --build "build-cxx$std"
  ctest --test-dir "build-cxx$std" --output-on-failure
done
python3 tests/mutations.py
