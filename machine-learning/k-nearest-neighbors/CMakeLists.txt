cmake_minimum_required(VERSION 3.16)
project(k_nearest_neighbors CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/W4 /WX /permissive-)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(knn_wine src/knn_wine.cpp)    # the article's listing
add_executable(knn_bench src/knn_bench.cpp)  # timing, not run by CTest
add_executable(test_knn tests/test_knn.cpp)

set(WINE_CSV ${CMAKE_CURRENT_SOURCE_DIR}/data/wine_split.csv)
enable_testing()
add_test(NAME test_knn COMMAND test_knn ${WINE_CSV})
# tests/expected/wine_knn.txt is the same file the Python version is checked against
add_test(NAME wine_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:knn_wine>
  -DARG=${WINE_CSV}
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/wine_knn.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
add_test(NAME bench_checksum COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:knn_bench>
  -DARG=1
  -DCONTAINS=checksum\ of\ predictions:\ 502492\;
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
