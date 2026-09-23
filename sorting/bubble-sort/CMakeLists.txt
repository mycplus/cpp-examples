cmake_minimum_required(VERSION 3.16)
project(bubble_sort_cpp CXX)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX /permissive-)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(bubble_sort      src/bubble_sort.cpp)
add_executable(test_bubble_sort tests/test_bubble_sort.cpp)

enable_testing()
add_test(NAME stability_and_containers COMMAND test_bubble_sort)
add_test(NAME output_bubble_sort
         COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:bubble_sort>
                 -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/bubble_sort.txt
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
