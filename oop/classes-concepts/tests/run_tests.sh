#!/usr/bin/env bash
# run_tests.sh - build and run the C++, Java and Python versions of the
# examples and check that each prints exactly what the article shows.
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

"$CXX" -std=c++17 -O2 -Wall -Wextra -pedantic -Werror -o "$OUT/temperature" "$HERE/cpp/temperature.cpp"
check "temperature (C++, $CXX)" "$EXP/temperature.txt" "$OUT/temperature"

javac -Xlint:all -Werror -d "$OUT/java" "$HERE/java/Temperature.java"
check "temperature (Java)" "$EXP/temperature.txt" java -cp "$OUT/java" Temperature

check "temperature (Python)"             "$EXP/temperature.txt"             python3 -W error "$HERE/python/temperature.py"
check "class_attribute_pitfall (Python)" "$EXP/class_attribute_pitfall.txt" python3 -W error "$HERE/python/class_attribute_pitfall.py"
check "name_mangling (Python)"           "$EXP/name_mangling.txt"           python3 -W error "$HERE/python/name_mangling.py"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
