cmake_minimum_required(VERSION 3.16)
project(library_sorts CXX)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(library_sorts src/library_sorts.cpp)
add_executable(test_library_sorts tests/test_library_sorts.cpp)

enable_testing()
add_test(NAME stable_sort_and_agreement COMMAND test_library_sorts)
# library_sorts prints timings, which are not a pass/fail property. It still
# runs: it exits non-zero if any sort returns a wrong result.
add_test(NAME library_sorts_runs COMMAND library_sorts)

add_executable(stable_vs_sort src/stable_vs_sort.cpp)
# Where std::sort puts equal keys is up to the library. The expected output is
# libstdc++'s, so the byte-for-byte check runs only on non-MSVC toolchains.
if(NOT MSVC)
add_test(NAME stable_vs_sort_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:stable_vs_sort>
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/stable_vs_sort.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endif()
