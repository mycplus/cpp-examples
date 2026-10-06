#!/usr/bin/env bash
# sanitizers.sh <compiler> [extra flags] - the correct examples are clean under
# AddressSanitizer and UndefinedBehaviorSanitizer, the invalidated iterator is
# reported, and the two compile-fail tests are rejected for the right reason.
set -uo pipefail

cxx=$1
extra=${2:-}
here=$(cd "$(dirname "$0")" && pwd)
src="$here/../src"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
fail=0

for f in first_look containers adaptors lookup_cost stable_positions map_lookup \
         function_objects erase_remove ranges_pipeline stream_iterators \
         algorithm_counts modern_adapters; do
    $cxx $extra -std=c++20 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
        "$src/$f.cpp" -o "$work/$f" || { echo "FAIL build $f"; fail=1; continue; }
    if "$work/$f" >/dev/null 2>"$work/$f.err"; then
        echo "PASS $f: no sanitizer report"
    else
        echo "FAIL $f: sanitizer report"; cat "$work/$f.err"; fail=1
    fi
done

$cxx $extra -std=c++17 -g -fsanitize=address "$here/sanitizer-fail/invalidated_iterator.cpp" \
    -o "$work/invalidated" || { echo "FAIL build invalidated_iterator"; fail=1; }
"$work/invalidated" >/dev/null 2>"$work/inv.err" || true   # a report means a non-zero exit
if grep -q "heap-use-after-free" "$work/inv.err"; then
    echo "PASS invalidated_iterator: heap-use-after-free reported"
else
    echo "FAIL invalidated_iterator: no report"; cat "$work/inv.err"; fail=1
fi

if $cxx $extra -std=c++17 -fsyntax-only "$here/compile-fail/sort_list.cpp" 2>/dev/null; then
    echo "FAIL sort_list compiled"; fail=1
else
    echo "PASS sort_list: rejected"
fi
if $cxx $extra -std=c++20 -fsyntax-only "$here/compile-fail/ranges_sort_list.cpp" 2>"$work/r.err"; then
    echo "FAIL ranges_sort_list compiled"; fail=1
elif grep -q "random_access" "$work/r.err"; then
    echo "PASS ranges_sort_list: rejected, naming random_access"
else
    echo "FAIL ranges_sort_list: rejected for another reason"; cat "$work/r.err"; fail=1
fi

exit $fail
