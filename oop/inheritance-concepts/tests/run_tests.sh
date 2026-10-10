#!/usr/bin/env bash
# run_tests.sh - build and run the C++, Java and Python versions of each
# example and check that all three print exactly what the article shows.
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

# C++: warnings are errors.
for prog in notifications fragile_base; do
    "$CXX" -std=c++17 -O2 -Wall -Wextra -pedantic -Werror -o "$OUT/$prog" "$HERE/cpp/$prog.cpp"
    check "$prog (C++, $CXX)" "$EXP/$prog.txt" "$OUT/$prog"
done

# Java: all lint warnings are errors.
javac -Xlint:all -Werror -d "$OUT/java" "$HERE/java/Notifications.java" "$HERE/java/FragileBase.java"
check "notifications (Java)" "$EXP/notifications.txt" java -cp "$OUT/java" Notifications
check "fragile_base (Java)"  "$EXP/fragile_base.txt"  java -cp "$OUT/java" FragileBase

# Python: warnings are errors.
check "notifications (Python)"        "$EXP/notifications.txt"        python3 -W error "$HERE/python/notifications.py"
check "fragile_base (Python)"         "$EXP/fragile_base.txt"         python3 -W error "$HERE/python/fragile_base.py"
check "composition (Python)"          "$EXP/composition.txt"          python3 -W error "$HERE/python/composition.py"
check "types_of_inheritance (Python)" "$EXP/types_of_inheritance.txt" python3 -W error "$HERE/python/types_of_inheritance.py"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
