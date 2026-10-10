#!/usr/bin/env bash
# run_tests.sh - build every example with warnings as errors, run it, compare
# its output with the article, check the warning the article quotes, and
# confirm that the compile-fail examples are rejected for the right reason.
#
# usage: bash tests/run_tests.sh g++|clang++ [c++17|c++20]
set -euo pipefail

CXX=${1:-g++}
STD=${2:-c++17}
HERE=$(cd "$(dirname "$0")/.." && pwd)
SRC=$HERE/src
EXP=$HERE/tests/expected
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
# Diagnostics are matched as text; under a UTF-8 locale GCC quotes names with
# curly quotes, so force the C locale for every compiler call.
export LC_ALL=C

FLAGS=(-std="$STD" -O2 -Wall -Wextra -pedantic -Werror)
fails=0
pass() { printf 'PASS  %s\n' "$1"; }
fail() { printf 'FAIL  %s\n' "$1"; fails=$((fails + 1)); }

compare() {
    if diff <("$OUT/$1" | tr -d '\r') "$EXP/$1.txt" >"$OUT/diff.txt"; then
        pass "$1 ($CXX, $STD)"
    else
        fail "$1 ($CXX, $STD)"; cat "$OUT/diff.txt"
    fi
}

# 1. Programs: no warnings, output identical to the article.
for prog in interfaces name_clash diamond virtual_base; do
    "$CXX" "${FLAGS[@]}" -o "$OUT/$prog" "$SRC/$prog.cpp"
    compare "$prog"
done

# 2. base_order exists to show the -Wreorder warning: confirm it still
#    appears, then build without it and compare the output.
log=$("$CXX" -std="$STD" -Wall -c "$SRC/base_order.cpp" -o "$OUT/bo.o" 2>&1 || true)
if grep -qE -- "-Wreorder" <<<"$log"; then
    pass "base_order warns -Wreorder"
else
    fail "base_order no longer warns"; head -5 <<<"$log"
fi
"$CXX" "${FLAGS[@]}" -Wno-reorder -o "$OUT/base_order" "$SRC/base_order.cpp"
compare base_order

# 3. Compile-fail examples must be rejected, and for the reason they exist.
expect_error() {   # expect_error <file> <regex the diagnostic must match>
    local file=$1 pattern=$2 log
    if log=$("$CXX" -std="$STD" -c "$HERE/tests/compile-fail/$file" -o "$OUT/cf.o" 2>&1); then
        fail "$file compiled but must not"
    elif grep -qE -- "$pattern" <<<"$log"; then
        pass "$file is rejected"
    else
        fail "$file failed for an unexpected reason"; head -5 <<<"$log"
    fi
}
expect_error ambiguous_member.cpp "request for member .power. is ambiguous|found in multiple base classes"
expect_error ambiguous_base.cpp   "is an ambiguous base of|ambiguous conversion from derived class"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
