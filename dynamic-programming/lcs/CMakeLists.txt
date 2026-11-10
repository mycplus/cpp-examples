cmake_minimum_required(VERSION 3.16)
project(lcs CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX /permissive-)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(lcs src/lcs.cpp)                  # the article's listing
add_executable(test_lcs tests/test_lcs.cpp)      # #includes src/lcs.cpp

enable_testing()
add_test(NAME test_lcs COMMAND test_lcs)
add_test(NAME lcs_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:lcs>
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/lcs.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
