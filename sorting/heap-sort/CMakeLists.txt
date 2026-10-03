cmake_minimum_required(VERSION 3.16)
project(heap_sort CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(src)

add_executable(heap_demo src/heap_demo.cpp)
add_executable(test_heap_sort tests/test_heap_sort.cpp)

enable_testing()
add_test(NAME test_heap_sort COMMAND test_heap_sort)

# The heap layouts printed by std::make_heap are libstdc++'s; the standard
# does not fix them, so the byte-for-byte check runs only off MSVC.
if(NOT MSVC)
add_test(NAME heap_demo_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:heap_demo>
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/heap_demo.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endif()
