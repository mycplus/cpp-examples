#!/usr/bin/env bash
# run_tests.sh <build-dir> [--portable]
# Runs every example and compares its output with tests/expected/.
# --portable skips outputs that depend on the standard library implementation
# (comparison counts, and the unspecified tail left by std::remove).
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

# Runs an example and strips the \r that MSVC's text-mode stdout adds.
run() {
    local path
    path=$(exe "$1")
    shift
    (cd "$build" && "$path" "$@") | tr -d '\r'
}

check_output() {
    local name=$1 out expected
    out=$(run "$name")
    expected=$(tr -d '\r' < "$here/expected/$name.txt")
    if [ "$out" = "$expected" ]; then
        echo "PASS $name"
    else
        echo "FAIL $name: output differs from expected/$name.txt"
        diff <(printf '%s\n' "$out") <(printf '%s\n' "$expected") || true
        fail=1
    fi
}

for name in first_look containers adaptors stable_positions map_lookup \
            function_objects ranges_pipeline stream_iterators modern_adapters; do
    check_output "$name"
done

if [ "$portable" != "--portable" ]; then
    check_output lookup_cost
    check_output erase_remove
    check_output algorithm_counts
else
    # The parts every conforming library must agree on.
    if run lookup_cost | grep -q 'std::find on vector: *765433 comparisons'; then
        echo "PASS lookup_cost (linear search count)"
    else
        echo "FAIL lookup_cost"; fail=1
    fi
    if run erase_remove | grep -q 'after erase     size 4: 1 2 3 4'; then
        echo "PASS erase_remove (result after erase)"
    else
        echo "FAIL erase_remove"; fail=1
    fi
    # count_if and find_if have exact operation counts in the standard;
    # the sort comparison count depends on the library.
    out=$(run algorithm_counts)
    if grep -q 'predicate called 1000 times for 1000 elements' <<< "$out" &&
       grep -q 'first match at index 2, predicate called 3 times' <<< "$out"; then
        echo "PASS algorithm_counts (count_if and find_if)"
    else
        echo "FAIL algorithm_counts"; echo "$out"; fail=1
    fi
fi

exit $fail
