#!/bin/sh
# run_all.sh - every Linux check behind the article's "What I Verified" table:
# GCC and Clang builds in C++17 and C++20 with warnings as errors, CTest,
# AddressSanitizer and UndefinedBehaviorSanitizer, deliberate mistakes that a
# test must catch, and one run of the benchmark.
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
  ctest --test-dir "$2" >>"$2.log" 2>&1 && echo passed || echo tests-failed
}

for cxx in g++ clang++; do
  for std in 17 20; do
    r=$(check . "$work/$cxx-$std" "$cxx" "$std")
    row "$cxx C++$std build and CTest" "$r"
    [ "$r" = passed ] || status=1
  done
done
r=$(check . "$work/sanitize" g++ 17 "-fsanitize=address,undefined -fno-sanitize-recover=all")
row "g++ C++17 ASan + UBSan, every test" "$r"
[ "$r" = passed ] || status=1

# A mutant only counts as caught if the unchanged code passes first.
if [ $status -ne 0 ]; then
  row "Mutants" "not run: the unchanged code fails its tests"; rm -rf "$work"; exit 1
fi

# Deliberate mistakes: number, file, text to find, replacement, description.
mutant() {
  n=$1; file=$2; from=$3; to=$4; what=$5
  dir="$work/mutant-$n"
  mkdir -p "$dir" && cp -r src tests bench CMakeLists.txt "$dir/"
  if ! grep -qF -- "$from" "$dir/$file"; then row "Mutant $n: $what" "NOT APPLIED"; status=1; return; fi
  f=$(printf '%s' "$from" | sed 's/[][\.*^$/&]/\\&/g')
  t=$(printf '%s' "$to" | sed 's/[\/&]/\\&/g')
  sed -i "s/$f/$t/" "$dir/$file"
  case $(check "$dir" "$dir/build" g++ 17) in
    tests-failed) row "Mutant $n: $what" CAUGHT ;;
    passed)       row "Mutant $n: $what" MISSED; status=1 ;;
    *)            row "Mutant $n: $what" "BUILD FAILED"; status=1 ;;
  esac
}
mutant 1 src/typeid_basics.cpp "virtual ~Shape() = default;" "~Shape() = default;" "Shape is no longer polymorphic"
mutant 2 src/dynamic_cast_basics.cpp "} catch (const std::bad_cast&) {" "} catch (const std::bad_typeid&) {" "the wrong exception type caught"
mutant 3 src/alternatives.cpp "if (auto* r = dynamic_cast<const Rect*>(&s))" "if (auto* r = dynamic_cast<const Rect*>(&s); r && r->w < 0)" "a missing branch in the dynamic_cast chain"
mutant 4 tests/compile_fail/not_polymorphic.cpp "struct Point { int x = 0; };" "struct Point { int x = 0; virtual ~Point() = default; };" "compile-fail program made valid"

g++ -std=c++17 -O2 -o "$work/cast_cost" bench/cast_cost.cpp && "$work/cast_cost"
rm -rf "$work"
exit $status
