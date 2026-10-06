#!/usr/bin/env bash
# sanitizers.sh <compiler> - which invalidation bugs AddressSanitizer and
# libstdc++'s debug mode report. Asserts only the cases where a report is
# expected (or where the code is valid); the silent cases are printed as INFO,
# because a tool's silence on undefined behaviour is not something to test for.
set -uo pipefail

cxx=$1
here=$(cd "$(dirname "$0")" && pwd)
src="$here/../src"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
fail=0

$cxx -std=c++17 -g -fsanitize=address "$src/invalidation_tools.cpp" -o "$work/asan" || exit 1
$cxx -std=c++17 -g -D_GLIBCXX_DEBUG "$src/invalidation_tools.cpp" -o "$work/debug" || exit 1

check() {   # tool mode expectation(report|clean|info) pattern
    local tool=$1 mode=$2 want=$3 pattern=$4 out
    out=$("$work/$tool" "$mode" 2>&1 || true)
    if grep -q "$pattern" <<< "$out"; then got=report; else got=clean; fi
    if [ "$want" = info ]; then
        echo "INFO $tool $mode: $got"
    elif [ "$want" = "$got" ]; then
        echo "PASS $tool $mode: $got"
    else
        echo "FAIL $tool $mode: expected $want, got $got"; echo "$out" | head -5; fail=1
    fi
}

check asan  vector          report "heap-use-after-free"
check asan  deque-reference clean  "ERROR: AddressSanitizer"
check asan  deque-iterator  info   "ERROR: AddressSanitizer"
check asan  deque-walk      report "heap-use-after-free"
check asan  unordered       info   "ERROR: AddressSanitizer"
check debug vector          report "attempt to dereference a singular iterator"
check debug deque-reference clean  "Error:"
check debug deque-iterator  report "attempt to dereference a singular iterator"
check debug deque-walk      report "attempt to"
check debug unordered       info   "Error:"

for f in list_operations list_sort_moves; do
    $cxx -std=c++17 -g -fsanitize=address,undefined "$src/$f.cpp" -o "$work/$f" || { fail=1; continue; }
    if "$work/$f" >/dev/null 2>"$work/$f.err"; then echo "PASS $f: no sanitizer report"
    else echo "FAIL $f"; cat "$work/$f.err"; fail=1; fi
done
exit $fail
