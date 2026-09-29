cmake_minimum_required(VERSION 3.16)
project(prime_numbers_cpp CXX)

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

add_executable(primes      src/primes.cpp)
add_executable(test_primes tests/test_primes.cpp)

enable_testing()
add_test(NAME primes_unit COMMAND test_primes)
add_test(NAME output_primes
         COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:primes>
                 -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/primes.txt
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
