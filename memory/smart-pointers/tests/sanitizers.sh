#!/usr/bin/env bash
# sanitizers.sh <compiler> - confirms that the sanitizers report exactly the
# defects the article describes, and nothing in the correct examples.
set -uo pipefail

cxx=$1
here=$(cd "$(dirname "$0")" && pwd)
src="$here/../src"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
fail=0

asan() { "$cxx" -std=c++17 -g -fsanitize=address,undefined -fno-omit-frame-pointer -pthread "$@"; }

expect_clean() {   # file
    local name; name=$(basename "$1" .cpp)
    asan "$1" -o "$work/$name" || { echo "FAIL build $name"; fail=1; return; }
    if (cd "$work" && "./$name" >/dev/null 2>"$work/$name.err"); then
        echo "PASS $name: no sanitizer report"
    else
        echo "FAIL $name: unexpected sanitizer report"; cat "$work/$name.err"; fail=1
    fi
}

expect_report() {  # file pattern [args...]
    local file=$1 pattern=$2; shift 2
    local name; name=$(basename "$file" .cpp)
    asan "$file" -o "$work/$name" || { echo "FAIL build $name"; fail=1; return; }
    (cd "$work" && "./$name" "$@" >/dev/null 2>"$work/$name.err") || true   # a report means a non-zero exit
    if grep -q "$pattern" "$work/$name.err"; then
        echo "PASS $name: reported '$pattern'"
    else
        echo "FAIL $name: expected '$pattern'"; cat "$work/$name.err"; fail=1
    fi
}

for f in unique_basics shared_basics cycle_fixed file_handle shared_from_this sizes allocations; do
    expect_clean "$src/$f.cpp"
done
expect_report "$src/raw_vs_unique.cpp" "Direct leak of 32 byte"

# The shared_ptr cycle is NOT asserted here. LeakSanitizer scans memory
# conservatively, and whether it reports this leak depends on the compiler,
# the optimisation level and the other sanitizers enabled (the article gives
# the measured matrix). run_tests.sh checks the leak deterministically: the
# expected output contains no destructor message.
asan "$src/cycle_leak.cpp" -o "$work/cycle_leak"
if (cd "$work" && ./cycle_leak >/dev/null 2>"$work/cycle_leak.err"); then
    echo "INFO cycle_leak: LeakSanitizer did not report the cycle in this build"
else
    echo "INFO cycle_leak: LeakSanitizer reported the cycle in this build"
fi
expect_report "$here/sanitizer-fail/two_owners.cpp" "attempting double-free"
expect_report "$src/long_list.cpp" "stack-overflow" 1000000 recursive

# ThreadSanitizer: copies are safe, assigning one shared_ptr from two threads is not.
"$cxx" -std=c++17 -g -O1 -fsanitize=thread -pthread "$src/threads.cpp" -o "$work/threads_tsan"
if "$work/threads_tsan" >/dev/null 2>"$work/t1.err"; then echo "PASS threads: no data race on copies"; else echo "FAIL threads"; cat "$work/t1.err"; fail=1; fi
"$work/threads_tsan" assign >/dev/null 2>"$work/t2.err" || true
if grep -q "data race" "$work/t2.err"; then echo "PASS threads assign: data race reported"; else echo "FAIL threads assign: no report"; fail=1; fi
"$cxx" -std=c++20 -g -O1 -fsanitize=thread -pthread "$src/atomic_shared.cpp" -o "$work/atomic_tsan"
if "$work/atomic_tsan" >/dev/null 2>"$work/t3.err"; then echo "PASS atomic_shared: no data race"; else echo "FAIL atomic_shared"; cat "$work/t3.err"; fail=1; fi

# Copying a std::unique_ptr must not compile.
if "$cxx" -std=c++17 -fsyntax-only "$here/compile-fail/copy_unique.cpp" 2>"$work/cf.err"; then
    echo "FAIL copy_unique compiled"; fail=1
elif grep -q "deleted" "$work/cf.err"; then
    echo "PASS copy_unique: rejected (deleted copy constructor)"
else
    echo "FAIL copy_unique: rejected for another reason"; cat "$work/cf.err"; fail=1
fi

exit $fail
