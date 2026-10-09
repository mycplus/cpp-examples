#!/usr/bin/env bash
# sanitizers.sh - every correct example must run clean under AddressSanitizer
# and UndefinedBehaviorSanitizer, and the dangling reference must be reported.
#
# usage: bash tests/sanitizers.sh g++|clang++
set -euo pipefail

CXX=${1:-g++}
HERE=$(cd "$(dirname "$0")/.." && pwd)
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
export ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1
SAN=(-std=c++17 -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined
     -fno-sanitize-recover=undefined -w)
fails=0

for prog in declare_define passing defaults overloading eval_order variadic returning function_values constexpr_fn; do
    "$CXX" "${SAN[@]}" -o "$OUT/$prog" "$HERE/src/$prog.cpp"
    status=0
    "$OUT/$prog" >/dev/null 2>"$OUT/err" || status=$?
    if [ "$status" -eq 0 ] && ! grep -qE "Sanitizer|runtime error" "$OUT/err"; then
        printf 'PASS  %s is clean\n' "$prog"
    else
        printf 'FAIL  %s (exit %s)\n' "$prog" "$status"; head -5 "$OUT/err"
        fails=$((fails + 1))
    fi
done

# The dangling reference, under AddressSanitizer alone (as in the article):
# Clang's build reports stack-use-after-return; GCC's build reads through the
# null pointer GCC substitutes for the returned reference, reported as a SEGV.
"$CXX" -std=c++17 -g -O1 -fno-omit-frame-pointer -fsanitize=address -w \
    -o "$OUT/dangling_return" "$HERE/src/dangling_return.cpp"
status=0
"$OUT/dangling_return" >/dev/null 2>"$OUT/err" || status=$?
if [ "$status" -ne 0 ] && grep -qE "AddressSanitizer: (stack-use-after-(return|scope)|SEGV)" "$OUT/err"; then
    printf 'PASS  dangling_return is reported: %s\n' "$(grep -oE 'AddressSanitizer: [a-zA-Z-]+' "$OUT/err" | head -1)"
else
    printf 'FAIL  dangling_return was not reported (exit %s)\n' "$status"; head -5 "$OUT/err"
    fails=$((fails + 1))
fi

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all sanitizer checks passed"
