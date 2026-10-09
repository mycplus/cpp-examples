#!/usr/bin/env bash
# sanitizers.sh - the correct examples must run clean under AddressSanitizer
# and UndefinedBehaviorSanitizer; every deliberate bug must be reported.
#
# usage: bash tests/sanitizers.sh g++|clang++
set -euo pipefail

CXX=${1:-g++}
HERE=$(cd "$(dirname "$0")/.." && pwd)
SRC=$HERE/src
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT

if "$CXX" --version | grep -qi clang; then IS_CLANG=1; else IS_CLANG=0; fi
SAN=(-std=c++17 -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=undefined -w)
export ASAN_OPTIONS=detect_leaks=1
fails=0
pass() { printf 'PASS  %s\n' "$1"; }
fail() { printf 'FAIL  %s\n' "$1"; fails=$((fails + 1)); }

clean() {     # clean <name> <command...>: exit 0 and no sanitizer report
    local name=$1; shift
    local status=0
    "$@" >/dev/null 2>"$OUT/err" || status=$?
    if [ "$status" -eq 0 ] && ! grep -qE "Sanitizer|runtime error" "$OUT/err"; then
        pass "$name is clean"
    else
        fail "$name (exit $status)"; head -5 "$OUT/err"
    fi
}

reports() {   # reports <name> <regex> <command...>: non-zero exit and a matching report
    local name=$1 pattern=$2; shift 2
    local status=0
    "$@" >/dev/null 2>"$OUT/err" || status=$?
    if [ "$status" -ne 0 ] && grep -qE "$pattern" "$OUT/err"; then
        pass "$name reported: $(grep -oE "$pattern" "$OUT/err" | head -1)"
    else
        fail "$name (exit $status), expected /$pattern/"; head -5 "$OUT/err"
    fi
}

for prog in ownership exception_safety shallow_copy dangling_temporary memory_errors; do
    "$CXX" "${SAN[@]}" -o "$OUT/$prog" "$SRC/$prog.cpp"
done
# ASan's allocator aborts on requests above its maximum instead of throwing
# std::bad_alloc, so the allocation-failure example runs under UBSan only.
"$CXX" -std=c++17 -g -O1 -fsanitize=undefined -fno-sanitize-recover=undefined -w \
    -o "$OUT/new_delete" "$SRC/new_delete.cpp"

clean "new_delete (UBSan)" "$OUT/new_delete"
clean "ownership" "$OUT/ownership"
reports "exception_safety RawEngine" "32 byte\(s\) leaked in 1 allocation" "$OUT/exception_safety"
reports "shallow_copy" "heap-use-after-free|double-free" "$OUT/shallow_copy"
reports "dangling_temporary" "heap-use-after-free|stack-use-after-scope" "$OUT/dangling_temporary"

ME=$OUT/memory_errors
reports "leak"                "LeakSanitizer: detected memory leaks"  "$ME" leak
reports "use-after-free"      "heap-use-after-free"                   "$ME" use-after-free
reports "double-free"         "attempting double-free"                "$ME" double-free
reports "mismatched-delete"   "bad-free|alloc-dealloc-mismatch"       "$ME" mismatched-delete
reports "heap-overflow"       "heap-buffer-overflow"                  "$ME" heap-overflow
reports "invalidated-pointer" "heap-use-after-free"                   "$ME" invalidated-pointer
if [ "$IS_CLANG" = 1 ]; then
    # Reported as stack-use-after-return at -O0 and -scope at -O1 in testing.
    reports "return-local"    "stack-use-after-(return|scope)"        "$ME" return-local
    reports "delete-non-heap" "bad-free"                              "$ME" delete-non-heap
else
    # GCC returns a null reference here (UBSan says so), and its ASan report for
    # deleting a stack address was not reliable in testing; require only that
    # the sanitizer stops the program.
    reports "return-local"    "SEGV|reference binding to null pointer" "$ME" return-local
    reports "delete-non-heap" "AddressSanitizer"                      "$ME" delete-non-heap
fi

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all sanitizer checks passed"
