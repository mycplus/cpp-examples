cmake_minimum_required(VERSION 3.16)
project(insertion_sort CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(src)

add_executable(insertion_demo src/insertion_demo.cpp)
add_executable(test_insertion_sort tests/test_insertion_sort.cpp)

enable_testing()
add_test(NAME test_insertion_sort COMMAND test_insertion_sort)
add_test(NAME insertion_demo_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:insertion_demo>
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/insertion_demo.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
