#!/usr/bin/env bash
# sanitizers.sh - the correct examples must run without a sanitizer report,
# and deleting through a base pointer without a virtual destructor must be
# reported.
#
# usage: bash tests/sanitizers.sh g++|clang++
set -euo pipefail

CXX=${1:-g++}
HERE=$(cd "$(dirname "$0")/.." && pwd)
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
export ASAN_OPTIONS=detect_leaks=1
SAN=(-std=c++17 -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined
     -fno-sanitize-recover=undefined -w)
fails=0

# array_pitfall is undefined behavior that neither sanitizer detects; the
# article says so, and this check keeps that statement true.
for prog in basics access order inheriting_ctors hiding final_and_override virtual_dtor array_pitfall; do
    "$CXX" "${SAN[@]}" -o "$OUT/$prog" "$HERE/src/$prog.cpp"
    status=0
    "$OUT/$prog" >/dev/null 2>"$OUT/err" || status=$?
    if [ "$status" -eq 0 ] && ! grep -qE "Sanitizer|runtime error" "$OUT/err"; then
        printf 'PASS  %s: no report\n' "$prog"
    else
        printf 'FAIL  %s (exit %s)\n' "$prog" "$status"; head -5 "$OUT/err"
        fails=$((fails + 1))
    fi
done

# virtual_dtor with an argument deletes a PlainChild through a Plain*.
# GCC's build reports new-delete-type-mismatch; Clang 18's reports the leaked
# string buffer, because ~PlainChild never ran.
status=0
"$OUT/virtual_dtor" broken >/dev/null 2>"$OUT/err" || status=$?
if [ "$status" -ne 0 ] && grep -qE "new-delete-type-mismatch|LeakSanitizer: detected memory leaks" "$OUT/err"; then
    printf 'PASS  virtual_dtor broken is reported: %s\n' "$(grep -oE 'new-delete-type-mismatch|detected memory leaks' "$OUT/err" | head -1)"
else
    printf 'FAIL  virtual_dtor broken was not reported (exit %s)\n' "$status"; head -5 "$OUT/err"
    fails=$((fails + 1))
fi

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all sanitizer checks passed"
