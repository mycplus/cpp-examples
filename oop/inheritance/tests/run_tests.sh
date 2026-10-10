#!/usr/bin/env bash
# run_tests.sh - build every example with warnings as errors, run it, compare
# its output with the article, and confirm that the compile-fail examples are
# rejected for the reason they exist.
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

# 1. Programs: no warnings, output identical to the article. virtual_dtor runs
#    without arguments here, which skips its undefined-behavior branch.
for prog in basics access order inheriting_ctors hiding final_and_override array_pitfall virtual_dtor; do
    "$CXX" "${FLAGS[@]}" -o "$OUT/$prog" "$SRC/$prog.cpp"
    if diff <("$OUT/$prog" | tr -d '\r') "$EXP/$prog.txt" >"$OUT/diff.txt"; then
        pass "$prog ($CXX, $STD)"
    else
        fail "$prog ($CXX, $STD)"; cat "$OUT/diff.txt"
    fi
done

# 2. Compile-fail examples must be rejected, and for the reason they exist.
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
expect_error private_member.cpp       "is private within this context|is a private member of"
expect_error default_private_base.cpp "inaccessible within this context|is a private member of"
expect_error missing_base_ctor.cpp    "no matching function for call to .Base::Base\(\).|must explicitly initialize the base class"
expect_error hidden_overload.cpp      "no matching function for call to .ColorPrinter::print\(\).|too few arguments to function call"
expect_error derive_from_final.cpp    "cannot derive from .final. base|is marked .final."

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
