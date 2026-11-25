cmake_minimum_required(VERSION 3.16)
project(decision_tree CXX)

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

add_executable(predict_tree src/predict_tree.cpp)  # the article's listing
add_executable(tree_bench src/tree_bench.cpp)      # timing, run by CTest only for its checksum
add_executable(test_tree tests/test_tree.cpp)

set(TREE ${CMAKE_CURRENT_SOURCE_DIR}/models/tree.txt)
set(ROWS ${CMAKE_CURRENT_SOURCE_DIR}/data/breast_cancer_test.csv)
enable_testing()
add_test(NAME test_tree COMMAND test_tree ${TREE} ${ROWS})
# tests/expected/predict_exported.txt is the same file the Python version is checked against
add_test(NAME prediction_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:predict_tree>
  -DARG=${TREE}
  -DARG2=${ROWS}
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/predict_exported.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
add_test(NAME bench_checksum COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:tree_bench>
  -DARG=1
  -DCONTAINS=checksums:\ loaded\ 732426313,\ generated\ 732426313
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
set_tests_properties(bench_checksum PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
