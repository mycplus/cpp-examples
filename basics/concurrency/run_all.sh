#!/bin/sh
# run_all.sh - every Linux check behind the article's "What I Verified" table:
# GCC and Clang builds in C++17 and C++20 with warnings as errors, CTest,
# AddressSanitizer + UndefinedBehaviorSanitizer, ThreadSanitizer, the data
# race in "counter racy" reported by ThreadSanitizer, and deliberate mistakes
# that a test must catch.
# Run from this directory:  sh run_all.sh
set -u
cd "$(dirname "$0")"
work=$(mktemp -d)
status=0
row() { printf '%-58s %s\n' "$1" "$2"; }

# check <source dir> <build dir> <compiler> <std> [extra flags]
# prints passed, build-failed or tests-failed
check() {
  cmake -S "$1" -B "$2" -DCMAKE_CXX_COMPILER="$3" -DCMAKE_CXX_STANDARD="$4" \
        -DCMAKE_CXX_FLAGS="${5:-}" >"$2.log" 2>&1 &&
  cmake --build "$2" -j 4 >>"$2.log" 2>&1 || { echo build-failed; return; }
  ctest --test-dir "$2" --timeout 30 >>"$2.log" 2>&1 && echo passed || echo tests-failed
}

for cxx in g++ clang++; do
  for std in 17 20; do
    r=$(check . "$work/$cxx-$std" "$cxx" "$std")
    row "$cxx C++$std build and CTest" "$r"
    [ "$r" = passed ] || status=1
  done
done
r=$(check . "$work/asan" g++ 17 "-fsanitize=address,undefined -fno-sanitize-recover=all")
row "g++ C++17 ASan + UBSan, every test" "$r"; [ "$r" = passed ] || status=1
tsan="-fsanitize=thread -g -O1"
r=$(check . "$work/tsan" g++ 17 "$tsan")
row "g++ C++17 ThreadSanitizer, every test" "$r"; [ "$r" = passed ] || status=1

# The race must be reported: ThreadSanitizer exits with 66 when it finds one.
if [ -x "$work/tsan/counter" ]; then
  "$work/tsan/counter" racy >/dev/null 2>"$work/racy.log"
  if [ $? -eq 66 ] && grep -q "data race" "$work/racy.log"; then
    row "ThreadSanitizer reports the race in 'counter racy'" yes
  else
    row "ThreadSanitizer reports the race in 'counter racy'" NO; status=1
  fi
fi

# A mutant only counts as caught if the unchanged code passes first.
if [ $status -ne 0 ]; then
  row "Mutants" "not run: the unchanged code fails its tests"; rm -rf "$work"; exit 1
fi

# Deliberate mistakes, built with ThreadSanitizer so that races fail the tests.
mutant() {
  n=$1; file=$2; from=$3; to=$4; what=$5
  dir="$work/mutant-$n"
  mkdir -p "$dir" && cp -r src tests CMakeLists.txt "$dir/"
  if ! grep -qF -- "$from" "$dir/$file"; then row "Mutant $n: $what" "NOT APPLIED"; status=1; return; fi
  f=$(printf '%s' "$from" | sed 's/[][\.*^$/&]/\\&/g')
  t=$(printf '%s' "$to" | sed 's/[\/&]/\\&/g')
  sed -i "s/$f/$t/" "$dir/$file"
  case $(check "$dir" "$dir/build" g++ 17 "$tsan") in
    tests-failed) row "Mutant $n: $what" CAUGHT ;;
    passed)       row "Mutant $n: $what" MISSED; status=1 ;;
    *)            row "Mutant $n: $what" "BUILD FAILED"; status=1 ;;
  esac
}
mutant 1 src/counter.cpp "std::lock_guard<std::mutex> lock(m);" "(void)m;" "the counter's lock removed"
mutant 2 src/bounded_queue.h "not_empty_.notify_all();" "/* no wake-up */" "close() does not wake the consumers"
mutant 3 src/transfers.cpp "std::scoped_lock lock(from.m, to.m);" "std::lock_guard<std::mutex> l1(from.m); std::lock_guard<std::mutex> l2(to.m);" "locks taken one at a time (deadlock)"
mutant 4 src/cpp20_sync.cpp "std::counting_semaphore<2> slots(2);" "std::counting_semaphore<3> slots(3);" "semaphore allows 3 threads"

rm -rf "$work"
exit $status
