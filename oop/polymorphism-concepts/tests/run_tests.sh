#!/usr/bin/env bash
# run_tests.sh - build and run the C++, Java and Python examples and check
# that each prints exactly what the article shows. shapes prints the same in
# all three languages; overloading differs by language on purpose (C++ prints
# 6, Java prints 6.0), so each has its own expected file.
#
# usage: bash tests/run_tests.sh [g++|clang++]
set -euo pipefail

CXX=${1:-g++}
HERE=$(cd "$(dirname "$0")/.." && pwd)
EXP=$HERE/tests/expected
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
fails=0
pass() { printf 'PASS  %s\n' "$1"; }
fail() { printf 'FAIL  %s\n' "$1"; fails=$((fails + 1)); }

check() {   # check <label> <expected file> <command...>
    local label=$1 expected=$2; shift 2
    if diff <("$@" | tr -d '\r') "$expected" >"$OUT/diff.txt"; then
        pass "$label"
    else
        fail "$label"; cat "$OUT/diff.txt"
    fi
}

for prog in shapes overloading; do
    "$CXX" -std=c++17 -O2 -Wall -Wextra -pedantic -Werror -o "$OUT/$prog" "$HERE/cpp/$prog.cpp"
done
check "shapes (C++, $CXX)"      "$EXP/shapes.txt"          "$OUT/shapes"
check "overloading (C++, $CXX)" "$EXP/overloading.cpp.txt" "$OUT/overloading"

javac -Xlint:all -Werror -d "$OUT/java" "$HERE/java/Shapes.java" "$HERE/java/Overloading.java"
check "shapes (Java)"      "$EXP/shapes.txt"           java -cp "$OUT/java" Shapes
check "overloading (Java)" "$EXP/overloading.java.txt" java -cp "$OUT/java" Overloading

check "shapes (Python)"      "$EXP/shapes.txt"      python3 -B -W error "$HERE/python/shapes.py"
check "duck_typing (Python)" "$EXP/duck_typing.txt" python3 -B -W error "$HERE/python/duck_typing.py"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
