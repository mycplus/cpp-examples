cmake_minimum_required(VERSION 3.16)
project(sequence_alignment CXX)

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

add_executable(align src/align.cpp)              # the article's listing
add_executable(test_align tests/test_align.cpp)  # #includes src/align.cpp

enable_testing()
add_test(NAME test_align COMMAND test_align)
add_test(NAME align_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:align>
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/align.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
