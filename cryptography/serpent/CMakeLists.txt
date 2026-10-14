cmake_minimum_required(VERSION 3.16)
project(serpent CXX)

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

include_directories(include)

add_executable(serpent_demo src/serpent_demo.cpp)   # the article's program
add_executable(test_vectors tests/test_vectors.cpp)
add_executable(serpent_speed bench/serpent_speed.cpp)  # built, not run by ctest

enable_testing()
function(expect_output name expected prog)
  add_test(NAME ${name} COMMAND ${CMAKE_COMMAND}
    -DPROG=${prog} "-DARGS=${ARGN}"
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${expected}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endfunction()

expect_output(demo_output serpent_demo.txt $<TARGET_FILE:serpent_demo>)
expect_output(vectors test_vectors.txt $<TARGET_FILE:test_vectors>
              ${CMAKE_CURRENT_SOURCE_DIR}/tests/vectors/serpent_nessie.txt)
