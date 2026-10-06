#!/usr/bin/env bash
# run_tests.sh <build-dir> [--portable]
# Compares program output with tests/expected/. --portable skips the move count
# reported for std::sort, which depends on the standard library's algorithm.
set -euo pipefail

build=$(cd "$1" && pwd)
portable=${2:-}
here=$(cd "$(dirname "$0")" && pwd)
fail=0

exe() {
    for p in "$build/$1" "$build/$1.exe" "$build/Release/$1.exe" "$build/Debug/$1.exe"; do
        [ -x "$p" ] && { echo "$p"; return; }
    done
    echo "missing executable: $1" >&2; exit 1
}

# Strips the \r that MSVC's text-mode stdout adds.
run() {
    local path
    path=$(exe "$1")
    shift
    (cd "$build" && "$path" "$@") | tr -d '\r'
}

compare() {   # name expected-text actual-text
    if [ "$2" = "$3" ]; then
        echo "PASS $1"
    else
        echo "FAIL $1"
        diff <(printf '%s\n' "$3") <(printf '%s\n' "$2") || true
        fail=1
    fi
}

compare list_operations "$(tr -d '\r' < "$here/expected/list_operations.txt")" "$(run list_operations)"

if [ "$portable" != "--portable" ]; then
    compare list_sort_moves "$(tr -d '\r' < "$here/expected/list_sort_moves.txt")" "$(run list_sort_moves)"
else
    # Every library must make no copies or moves in list::sort, and the
    # mt19937 keys put the first element at the same sorted position.
    out=$(run list_sort_moves)
    if grep -q '^list::sort   copies 0, moves 0$' <<< "$out" &&
       grep -q 'new position 359)' <<< "$out"; then
        echo "PASS list_sort_moves (portable lines)"
    else
        echo "FAIL list_sort_moves"; echo "$out"; fail=1
    fi
fi

# The one invalidation case with defined behaviour: a deque reference
# survives push_back at the end.
compare "invalidation_tools deque-reference" "1" "$(run invalidation_tools deque-reference)"

exit $fail
