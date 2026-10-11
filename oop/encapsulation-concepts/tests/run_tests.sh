#!/usr/bin/env bash
# run_tests.sh - build and run the C++, Java and Python versions of the
# examples and check that each prints exactly what the article shows.
# account prints the same in all three languages; leak differs on its last
# line by design (each language protects the list differently), so each
# language has its own expected file.
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

for prog in account leak; do
    "$CXX" -std=c++17 -O2 -Wall -Wextra -pedantic -Werror -o "$OUT/$prog" "$HERE/cpp/$prog.cpp"
done
check "account (C++, $CXX)" "$EXP/account.txt"  "$OUT/account"
check "leak (C++, $CXX)"    "$EXP/leak.cpp.txt" "$OUT/leak"

javac -Xlint:all -Werror -d "$OUT/java" "$HERE/java/Account.java" "$HERE/java/Leak.java"
check "account (Java)" "$EXP/account.txt"   java -cp "$OUT/java" Account
check "leak (Java)"    "$EXP/leak.java.txt" java -cp "$OUT/java" Leak

check "account (Python)"    "$EXP/account.txt"     python3 -B -W error "$HERE/python/account.py"
check "leak (Python)"       "$EXP/leak.python.txt" python3 -B -W error "$HERE/python/leak.py"
check "properties (Python)" "$EXP/properties.txt"  python3 -B -W error "$HERE/python/properties.py"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
