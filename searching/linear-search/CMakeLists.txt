cmake_minimum_required(VERSION 3.16)
project(linear_search CXX)

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

add_executable(linear_search src/linear_search.cpp)   # the article's listing
add_executable(test_linear_search tests/test_linear_search.cpp)  # #includes the listing

enable_testing()
add_test(NAME test_linear_search COMMAND test_linear_search)
add_test(NAME linear_search_output COMMAND ${CMAKE_COMMAND}
  -DPROG=$<TARGET_FILE:linear_search>
  -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/linear_search.txt
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
