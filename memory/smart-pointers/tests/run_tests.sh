#!/usr/bin/env bash
# run_tests.sh <build-dir> [--portable]
# Runs every example and compares its output with tests/expected/.
# --portable skips the outputs that depend on libstdc++ (allocation sizes and
# the text of std::bad_weak_ptr::what()), for macOS and Windows runners.
set -euo pipefail

build=$(cd "$1" && pwd)
portable=${2:-}
here=$(cd "$(dirname "$0")" && pwd)
fail=0

exe() {   # locate an executable in single- and multi-config build trees
    for p in "$build/$1" "$build/$1.exe" "$build/Release/$1.exe" "$build/Debug/$1.exe"; do
        [ -x "$p" ] && { echo "$p"; return; }
    done
    echo "missing executable: $1" >&2; exit 1
}

check_output() {
    local name=$1
    local out
    out=$(cd "$build" && "$(exe "$name")")
    if diff --strip-trailing-cr <(printf '%s\n' "$out") "$here/expected/$name.txt" >/dev/null; then
        echo "PASS $name"
    else
        echo "FAIL $name: output differs from expected/$name.txt"
        diff --strip-trailing-cr <(printf '%s\n' "$out") "$here/expected/$name.txt" || true
        fail=1
    fi
}

for name in raw_vs_unique unique_basics shared_basics cycle_leak cycle_fixed file_handle; do
    check_output "$name"
done
if [ "$portable" != "--portable" ]; then
    check_output allocations
    check_output shared_from_this
else
    "$(exe shared_from_this)" >/dev/null && echo "PASS shared_from_this (exit status only)"
fi

if "$(exe long_list)" 1000000 iterative | grep -q '^destroyed$'; then
    echo "PASS long_list: 1,000,000 nodes destroyed iteratively"
else
    echo "FAIL long_list iterative"; fail=1
fi

"$(exe threads)" | grep -q '^copies: done' && echo "PASS threads (copies)" || { echo "FAIL threads"; fail=1; }
"$(exe atomic_shared)" | grep -q 'from a final store' && echo "PASS atomic_shared" || { echo "FAIL atomic_shared"; fail=1; }
"$(exe sizes)"

exit $fail
