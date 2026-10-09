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

compare() {   # compare <program> <expected file>
    if diff <("$OUT/$1" | tr -d '\r') "$2" >"$OUT/diff.txt"; then
        pass "$1 ($CXX, $STD)"
    else
        fail "$1 ($CXX, $STD)"; cat "$OUT/diff.txt"
    fi
}

# 1. Programs: no warnings, output identical to the article.
for prog in declare_define passing defaults overloading variadic returning function_values constexpr_fn; do
    "$CXX" "${FLAGS[@]}" -o "$OUT/$prog" "$SRC/$prog.cpp"
    compare "$prog" "$EXP/$prog.txt"
done

# 2. Argument evaluation order is unspecified: GCC and Clang differ, and the
#    article shows both. Apple Clang reports itself as clang.
"$CXX" "${FLAGS[@]}" -o "$OUT/eval_order" "$SRC/eval_order.cpp"
if "$CXX" --version | grep -qi clang; then family=clang; else family=gcc; fi
compare eval_order "$EXP/eval_order.$family.txt"

# 3. dangling_return.cpp exists to show the compiler's warning; it is never run
#    here (sanitizers.sh runs it under AddressSanitizer).
log=$("$CXX" -std="$STD" -Wall -c "$SRC/dangling_return.cpp" -o "$OUT/dr.o" 2>&1 || true)
# Capture first: grep -q in a pipeline exits early, and under pipefail the
# compiler's SIGPIPE would turn a match into a failure.
if grep -qE -- "-Wreturn-(local-addr|stack-address)" <<<"$log"; then
    pass "dangling_return warns"
else
    fail "dangling_return no longer warns"; head -5 <<<"$log"
fi

# 4. Compile-fail examples must be rejected, and for the reason they exist.
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
# Patterns match the diagnostic text, not the file names.
expect_error return_type_only.cpp     "ambiguating new declaration|differ only in their return type"
expect_error ambiguous_call.cpp       "is ambiguous"
expect_error default_redefined.cpp    "default argument given for parameter|redefinition of default argument"
expect_error default_not_trailing.cpp "default argument missing for parameter|missing default argument on parameter"
expect_error temporary_to_ref.cpp     "cannot bind non-const lvalue reference|expects an lvalue"
expect_error undeclared_call.cpp      "was not declared in this scope|use of undeclared identifier"

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
