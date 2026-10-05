cmake_minimum_required(VERSION 3.16)
project(knapsack CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(knapsack src/knapsack.cpp)
add_executable(test_knapsack tests/test_knapsack.cpp)   # #includes src/knapsack.cpp

enable_testing()
add_test(NAME test_knapsack COMMAND test_knapsack)
add_test(NAME knapsack_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:knapsack>
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/knapsack.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
