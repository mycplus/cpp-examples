#!/usr/bin/env bash
# run_tests.sh - build and run the C++, Java and Python versions of the
# shipping example and check that each prints exactly what the article shows.
# The object-oriented version prints the same in all three languages.
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

"$CXX" -std=c++17 -O2 -Wall -Wextra -pedantic -Werror -o "$OUT/shipping" "$HERE/cpp/shipping.cpp"
check "shipping (C++, $CXX)" "$EXP/shipping.txt" "$OUT/shipping"

javac -Xlint:all -Werror -d "$OUT/java" "$HERE/java/Shipping.java"
check "shipping (Java)" "$EXP/shipping.txt" java -cp "$OUT/java" Shipping

check "shipping (Python)"            "$EXP/shipping.txt"            python3 -B -W error "$HERE/python/shipping.py"
check "shipping_procedural (Python)" "$EXP/shipping_procedural.txt" python3 -B -W error "$HERE/python/shipping_procedural.py"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
