#!/usr/bin/env bash
# run_tests.sh - build and run the C++, Java and Python versions of the
# examples and check that each prints exactly what the article shows.
# identity differs by language on purpose, so each language has its own
# expected file; lifetime prints the same in all three.
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

for prog in identity lifetime; do
    "$CXX" -std=c++17 -O2 -Wall -Wextra -pedantic -Werror -o "$OUT/$prog" "$HERE/cpp/$prog.cpp"
done
check "identity (C++, $CXX)" "$EXP/identity.cpp.txt" "$OUT/identity"
check "lifetime (C++, $CXX)" "$EXP/lifetime.txt"     "$OUT/lifetime"

javac -Xlint:all -Werror -d "$OUT/java" "$HERE/java/Identity.java" "$HERE/java/Lifetime.java"
check "identity (Java)" "$EXP/identity.java.txt" java -cp "$OUT/java" Identity
check "lifetime (Java)" "$EXP/lifetime.txt"      java -cp "$OUT/java" Lifetime

check "identity (Python)"     "$EXP/identity.python.txt" python3 -B -W error "$HERE/python/identity.py"
check "lifetime (Python)"     "$EXP/lifetime.txt"        python3 -B -W error "$HERE/python/lifetime.py"
check "shallow_copy (Python)" "$EXP/shallow_copy.txt"    python3 -B -W error "$HERE/python/shallow_copy.py"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
