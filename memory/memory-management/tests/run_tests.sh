#!/usr/bin/env bash
# run_tests.sh - build the examples with warnings as errors, run them and
# compare their output with the output published in the article. Also checks
# the compiler warnings the article quotes, and that the deliberately broken
# programs fail.
#
# usage: bash tests/run_tests.sh g++|clang++ [c++17|c++20]
set -euo pipefail

CXX=${1:-g++}
STD=${2:-c++17}
HERE=$(cd "$(dirname "$0")/.." && pwd)
SRC=$HERE/src
EXP=$HERE/tests/expected
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT

if "$CXX" --version | grep -qi clang; then IS_CLANG=1; else IS_CLANG=0; fi
FLAGS=(-std="$STD" -O2 -Wall -Wextra -pedantic -Werror)
fails=0
pass() { printf 'PASS  %s\n' "$1"; }
fail() { printf 'FAIL  %s\n' "$1"; fails=$((fails + 1)); }

# Program output with any carriage returns removed, so CRLF never matters.
run() { "$@" 2>/dev/null | tr -d '\r'; }

compare() {   # compare <name> <expected-file> <command...>
    local name=$1 expected=$2; shift 2
    if diff <(run "$@") "$EXP/$expected" >"$OUT/diff.txt"; then
        pass "$name"
    else
        fail "$name"; cat "$OUT/diff.txt"
    fi
}

# 1. Correct programs: no warnings, output identical to the article.
for prog in new_delete ownership exception_safety; do
    "$CXX" "${FLAGS[@]}" -o "$OUT/$prog" "$SRC/$prog.cpp"
    compare "$prog ($CXX, $STD)" "$prog.txt" "$OUT/$prog"
done

# 2. throwing_destructor: the compiler's own warning is the point of the
#    example, so only that one warning is allowed.
if [ "$IS_CLANG" = 1 ]; then ALLOW=-Wno-exceptions; else ALLOW=-Wno-terminate; fi
"$CXX" "${FLAGS[@]}" "$ALLOW" -o "$OUT/throwing_destructor" "$SRC/throwing_destructor.cpp"
compare "throwing_destructor loose" "throwing_destructor_loose.txt" "$OUT/throwing_destructor" loose
status=0; "$OUT/throwing_destructor" >"$OUT/td.out" 2>"$OUT/td.err" || status=$?
if [ "$status" -eq 134 ] && grep -q "terminate called after throwing" "$OUT/td.err" \
   && ! grep -q "caught" "$OUT/td.out"; then
    pass "throwing_destructor default calls std::terminate"
else
    fail "throwing_destructor default (exit $status)"; cat "$OUT/td.out" "$OUT/td.err"
fi

# 3. Warnings quoted in the article.
expect_warning() {   # expect_warning <file> <flag-name> [extra flags...]
    local file=$1 flag=$2; shift 2
    local log
    log=$("$CXX" -std="$STD" -O2 -Wall -Wextra "$@" -c "$SRC/$file" -o /dev/null 2>&1 || true)
    # Capture first: grep -q in a pipeline exits early, and under pipefail the
    # compiler's SIGPIPE would turn a match into a failure.
    if grep -q -- "\[$flag" <<<"$log"; then
        pass "$file warns $flag"
    else
        fail "$file does not warn $flag"
    fi
}
expect_quiet() {     # expect_quiet <file>: no warnings under -Wall -Wextra
    local file=$1
    if [ -z "$("$CXX" -std="$STD" -O2 -Wall -Wextra -c "$SRC/$file" -o /dev/null 2>&1)" ]; then
        pass "$file compiles silently under -Wall -Wextra"
    else
        fail "$file produced warnings"
    fi
}

expect_quiet shallow_copy.cpp
if [ "$IS_CLANG" = 1 ]; then
    expect_warning memory_errors.cpp -Wmismatched-new-delete
    expect_warning memory_errors.cpp -Wreturn-stack-address
    expect_warning shallow_copy.cpp -Wdeprecated-copy-with-user-provided-dtor -Wdeprecated
    expect_quiet dangling_temporary.cpp
else
    expect_warning memory_errors.cpp -Wmismatched-new-delete
    expect_warning memory_errors.cpp -Wreturn-local-addr
    expect_warning memory_errors.cpp -Wfree-nonheap-object
    expect_warning memory_errors.cpp -Wuse-after-free
    expect_warning shallow_copy.cpp -Wdeprecated-copy-dtor -Wdeprecated-copy-dtor
    expect_warning dangling_temporary.cpp -Wdangling-reference
fi

# 4. The shallow copy must fail at run time (glibc detects the double free).
"$CXX" -std="$STD" -O2 -o "$OUT/shallow_copy" "$SRC/shallow_copy.cpp"
status=0; "$OUT/shallow_copy" >/dev/null 2>"$OUT/sc.err" || status=$?
if [ "$status" -ne 0 ]; then pass "shallow_copy fails at run time (exit $status)"
else fail "shallow_copy exited 0"; fi

# 5. memory_errors builds and rejects an unknown mode.
"$CXX" -std="$STD" -O2 -w -o "$OUT/memory_errors" "$SRC/memory_errors.cpp"
status=0; "$OUT/memory_errors" no-such-mode 2>/dev/null || status=$?
if [ "$status" -eq 2 ]; then pass "memory_errors usage check"; else fail "memory_errors usage (exit $status)"; fi

echo
if [ "$fails" -ne 0 ]; then echo "$fails check(s) failed"; exit 1; fi
echo "all checks passed"
