#!/bin/sh
# run_all.sh - every Linux check behind the article's "What I Verified" table:
# GCC and Clang builds in C++17 and C++20 with warnings as errors, CTest,
# AddressSanitizer and UndefinedBehaviorSanitizer, and deliberate mistakes
# that a test must catch.
# Run from this directory:  sh run_all.sh      (as root to include change_test;
# as a normal user, change_test reports "skipped")
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

# Deliberate mistakes: file, text to find, replacement, description.
mutant() {
  n=$1; file=$2; from=$3; to=$4; what=$5
  dir="$work/mutant-$n"
  mkdir -p "$dir" && cp -r src tests CMakeLists.txt "$dir/"
  if ! grep -qF -- "$from" "$dir/$file"; then row "Mutant $n: $what" "NOT APPLIED"; status=1; return; fi
  # Replace the text literally: escape sed's special characters first.
  f=$(printf '%s' "$from" | sed 's/[][\.*^$/&]/\\&/g')
  t=$(printf '%s' "$to" | sed 's/[\/&]/\\&/g')
  sed -i "s/$f/$t/" "$dir/$file"
  case $(check "$dir" "$dir/build" g++ 17) in
    tests-failed) row "Mutant $n: $what" CAUGHT ;;
    passed)       row "Mutant $n: $what" MISSED; status=1 ;;
    *)            row "Mutant $n: $what" "BUILD FAILED"; status=1 ;;
  esac
}
# A mutant only counts as caught if the unchanged code passes first.
if [ $status -ne 0 ]; then
  row "Mutants" "not run: the unchanged code fails its tests"; rm -rf "$work"; exit 1
fi
mutant 1 src/ip_text.cpp "if (j - i > best_length) {" "if (j - i >= best_length) {" "IPv6 compresses the last zero run"
mutant 2 src/ip_text.cpp "(s.size() > 1 && s[0] == '0')" "(false)" "leading zeros accepted in IPv4"
mutant 3 src/ip_text.cpp "if (bits != expected)" "if (bits == 0 && expected != 0)" "non-contiguous masks accepted"
mutant 4 src/interfaces_linux.cpp "for (int b = 7; b >= 0; --b)" "for (int b = 7; b > 0; --b)" "prefix length counts 7 bits a byte"
if [ "$(id -u)" = 0 ]; then
  mutant 5 src/interfaces_linux.cpp "NLM_F_CREATE | NLM_F_EXCL" "NLM_F_CREATE | NLM_F_REPLACE" "adding twice replaces instead of failing"
else
  row "Mutant 5: adding twice replaces instead of failing" "not run (needs root)"
fi

rm -rf "$work"
exit $status
