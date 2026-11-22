#!/bin/sh
# The Linux checks for the C++ version:
#   1. GCC and Clang, C++17 and C++20, warnings as errors, CTest.
#   2. AddressSanitizer and UBSan, and Valgrind.
#   3. Four deliberate mistakes in the library that the tests must catch.
set -u
cd "$(dirname "$0")"
status=0
tmp=$(mktemp -d)
report() { printf '%-62s %s\n' "$1" "$2"; }
build() { d=$1; shift; cmake -S . -B "$d" "$@" >"$d.log" 2>&1 && cmake --build "$d" -j 4 >>"$d.log" 2>&1; }

for cxx in g++ clang++; do
    for std in 17 20; do
        if build "$tmp/$cxx-$std" -DCMAKE_CXX_COMPILER=$cxx -DCMAKE_CXX_STANDARD=$std &&
           ctest --test-dir "$tmp/$cxx-$std" >/dev/null 2>&1; then
            report "$cxx C++$std build and CTest" PASS
        else
            report "$cxx C++$std build and CTest" FAIL; status=1
        fi
    done
done
"$tmp/g++-17/test_defensive" data "$tmp" | tail -1 | sed 's/^/unit tests: /'

san="-fsanitize=address,undefined -fno-sanitize-recover=all -g"
if build "$tmp/asan" -DCMAKE_CXX_COMPILER=g++ "-DCMAKE_CXX_FLAGS=$san" && ctest --test-dir "$tmp/asan" >/dev/null 2>&1; then
    report "g++ ASan and UBSan, CTest" PASS
else
    report "g++ ASan and UBSan, CTest" FAIL; status=1
fi
if command -v valgrind >/dev/null 2>&1; then
    if valgrind -q --error-exitcode=9 --leak-check=full "$tmp/g++-17/test_defensive" data "$tmp" >/dev/null 2>&1 &&
       { valgrind -q --error-exitcode=9 --leak-check=full "$tmp/g++-17/people" data/people.txt >/dev/null 2>&1; [ $? -ne 9 ]; }; then
        report "Valgrind memcheck: unit tests and people" "no errors, no leaks"
    else
        report "Valgrind memcheck: unit tests and people" "REPORTED A PROBLEM"; status=1
    fi
fi

mutant() { # label, sed expression on src/defensive.cpp
    rm -rf "$tmp/m" && mkdir "$tmp/m" && cp -r CMakeLists.txt src tests data pitfalls compile_fail "$tmp/m/"
    sed -i "$2" "$tmp/m/src/defensive.cpp"
    if cmp -s src/defensive.cpp "$tmp/m/src/defensive.cpp"; then report "$1" "NOT APPLIED"; status=1; return; fi
    if build "$tmp/mb" -DCMAKE_CXX_COMPILER=g++ -S "$tmp/m" && ctest --test-dir "$tmp/mb" >/dev/null 2>&1; then
        report "$1" MISSED; status=1
    else
        report "$1" CAUGHT
    fi
    rm -rf "$tmp/mb"
}
mutant "Mutant 1: text after a number accepted" 's/if (end != text.data() + text.size()) {/if (false) {/'
mutant "Mutant 2: a line of exactly max_len called too long" 's/if (text.size() > max_len) {/if (text.size() >= max_len) {/'
mutant "Mutant 3: no overflow check before multiplying" 's/if (b != 0 \&\& a > std::numeric_limits<std::size_t>::max() \/ b) {/if (false) {/'
mutant "Mutant 4: the last partial block of a copy dropped" 's/ || in.gcount() > 0) {/) {/'

rm -rf "$tmp"
exit $status
