#!/usr/bin/env bash
# run_tests.sh - build every example with warnings as errors, run it, compare
# its output with the article, check the warnings the article quotes, and
# confirm that the compile-fail examples are rejected.
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

FLAGS=(-std="$STD" -O2 -Wall -Wextra -pedantic -Werror)
fails=0
pass() { printf 'PASS  %s\n' "$1"; }
fail() { printf 'FAIL  %s\n' "$1"; fails=$((fails + 1)); }

# 1. Programs: no warnings, output identical to the article.
for prog in library slicing operators variant_shapes; do
    "$CXX" "${FLAGS[@]}" -o "$OUT/$prog" "$SRC/$prog.cpp"
    if diff <("$OUT/$prog" | tr -d '\r') "$EXP/$prog.txt" >"$OUT/diff.txt"; then
        pass "$prog ($CXX, $STD)"
    else
        fail "$prog ($CXX, $STD)"; cat "$OUT/diff.txt"
    fi
done

# 2. missing_override exists to show -Woverloaded-virtual, so only that
#    warning is allowed; first confirm the compiler still gives it.
log=$("$CXX" -std="$STD" -O2 -Wall -Wextra -c "$SRC/missing_override.cpp" -o /dev/null 2>&1 || true)
# Capture first: grep -q in a pipeline exits early, and under pipefail the
# compiler's SIGPIPE would turn a match into a failure.
if grep -q -- "-Woverloaded-virtual" <<<"$log"; then
    pass "missing_override warns -Woverloaded-virtual"
else
    fail "missing_override no longer warns"
fi
"$CXX" "${FLAGS[@]}" -Wno-overloaded-virtual -o "$OUT/missing_override" "$SRC/missing_override.cpp"
if diff <("$OUT/missing_override" | tr -d '\r') "$EXP/missing_override.txt" >"$OUT/diff.txt"; then
    pass "missing_override output"
else
    fail "missing_override output"; cat "$OUT/diff.txt"
fi

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
# Patterns match the diagnostic text, not the file names (which contain the
# same words).
expect_error abstract_instance.cpp    "abstract (type|class)"
expect_error default_private.cpp      "is private|private member"
expect_error override_typo.cpp        "marked 'override'"
expect_error visitor_missing_case.cpp "invoke_result|exhaustive|no matching"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
