#!/bin/sh
# Builds and tests the JSON validation examples with GCC and Clang (plus
# AddressSanitizer and UBSan), then checks that the tests catch three
# deliberate mistakes. Set JSON_SOURCE_DIR to an unpacked nlohmann/json 3.12.0
# to build without downloading it.
set -u
cd "$(dirname "$0")"
here=$(pwd)
fetch=""
[ -n "${JSON_SOURCE_DIR:-}" ] && fetch="-DFETCHCONTENT_SOURCE_DIR_JSON=$JSON_SOURCE_DIR"
status=0

report() { printf '%-58s %s\n' "$1" "$2"; }

build_and_test() { # name, source dir, build dir, extra cmake args...
    name=$1; src=$2; dir=$3; shift 3
    if cmake -S "$src" -B "$dir" $fetch "$@" >"$dir.log" 2>&1 &&
       cmake --build "$dir" -j 4 >>"$dir.log" 2>&1 &&
       ctest --test-dir "$dir" --output-on-failure >>"$dir.log" 2>&1; then
        return 0
    fi
    return 1
}

for cxx in g++ clang++; do
    if build_and_test "$cxx" . "build-$cxx" -DCMAKE_CXX_COMPILER=$cxx; then
        report "$cxx build and tests" PASS
    else
        report "$cxx build and tests" FAIL; status=1
    fi
done
if build_and_test asan . build-asan -DCMAKE_CXX_COMPILER=g++ \
     "-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=all -g"; then
    report "g++ ASan and UBSan" PASS
else
    report "g++ ASan and UBSan" FAIL; status=1
fi

# Each mutant copies the sources, changes one thing and must make a test fail.
mutant() { # label, file, sed expression
    rm -rf mutant && mkdir mutant && cp -r CMakeLists.txt src tests cases mutant/
    sed -i "$3" "mutant/$2"
    if cmp -s "$2" "mutant/$2"; then report "$1" "NOT APPLIED"; status=1; return; fi
    if build_and_test mutant mutant build-mutant -DCMAKE_CXX_COMPILER=g++; then
        report "$1" MISSED; status=1
    else
        report "$1" CAUGHT
    fi
    rm -rf mutant build-mutant
}
mutant "Mutant 1: validator ignores comments" src/validate_json.cpp \
    's/json::parse(text);/json::parse(text, nullptr, true, true);/'
mutant "Mutant 2: unknown fields allowed" src/validate_user.cpp \
    's/if (known.count(item.key()) == 0)/if (false)/'
mutant "Mutant 3: score range not checked" src/validate_user.cpp \
    's/ || score.get<double>() < 0 || score.get<double>() > 100//'
cd "$here"
exit $status
