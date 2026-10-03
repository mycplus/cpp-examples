cmake_minimum_required(VERSION 3.16)
project(selection_sort CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(src)

add_executable(selection_demo src/selection_demo.cpp)
add_executable(test_selection_sort tests/test_selection_sort.cpp)

enable_testing()
add_test(NAME test_selection_sort COMMAND test_selection_sort)
add_test(NAME selection_demo_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:selection_demo>
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/selection_demo.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
