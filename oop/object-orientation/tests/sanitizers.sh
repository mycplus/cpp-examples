#!/usr/bin/env bash
# sanitizers.sh - every example must run clean under AddressSanitizer and
# UndefinedBehaviorSanitizer.
#
# usage: bash tests/sanitizers.sh g++|clang++
set -euo pipefail

CXX=${1:-g++}
HERE=$(cd "$(dirname "$0")/.." && pwd)
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
export ASAN_OPTIONS=detect_leaks=1
fails=0

for prog in library missing_override slicing operators variant_shapes; do
    "$CXX" -std=c++17 -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined \
        -fno-sanitize-recover=undefined -w -o "$OUT/$prog" "$HERE/src/$prog.cpp"
    status=0
    "$OUT/$prog" >/dev/null 2>"$OUT/err" || status=$?
    if [ "$status" -eq 0 ] && ! grep -qE "Sanitizer|runtime error" "$OUT/err"; then
        printf 'PASS  %s is clean\n' "$prog"
    else
        printf 'FAIL  %s (exit %s)\n' "$prog" "$status"; head -5 "$OUT/err"
        fails=$((fails + 1))
    fi
done

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all sanitizer checks passed"
