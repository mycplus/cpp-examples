cmake_minimum_required(VERSION 3.16)
project(towers_of_hanoi CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX /permissive-)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(include)

add_executable(towers_of_hanoi src/towers_of_hanoi.cpp)     # the article's listing
add_executable(hanoi_cli src/hanoi_cli.cpp)
add_executable(test_hanoi tests/test_hanoi.cpp)
add_executable(pitfall_print_in_one_statement pitfalls/print_in_one_statement.cpp)

enable_testing()
add_test(NAME test_hanoi COMMAND test_hanoi)

function(expect_output name expected prog)
  add_test(NAME ${name} COMMAND ${CMAKE_COMMAND}
    -DPROG=${prog} "-DARGS=${ARGN}"
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${expected}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endfunction()

expect_output(listing_output towers_of_hanoi.txt $<TARGET_FILE:towers_of_hanoi>)
expect_output(cli_recursive_output towers_of_hanoi.txt $<TARGET_FILE:hanoi_cli> 3)
expect_output(cli_iterative_output towers_of_hanoi.txt $<TARGET_FILE:hanoi_cli> "3,--iterative")
# C++17 evaluates the operands of << left to right, so this output is fixed.
expect_output(pitfall_print_in_one_statement_output pitfall_print_in_one_statement.txt
              $<TARGET_FILE:pitfall_print_in_one_statement>)

set(bad_args abc -1 21 3x 2.5)
foreach(arg IN LISTS bad_args)
  add_test(NAME cli_rejects_${arg} COMMAND hanoi_cli ${arg})
  set_tests_properties(cli_rejects_${arg} PROPERTIES WILL_FAIL TRUE)
endforeach()
add_test(NAME cli_rejects_empty COMMAND hanoi_cli "")
add_test(NAME cli_rejects_no_argument COMMAND hanoi_cli)
add_test(NAME cli_rejects_bad_flag COMMAND hanoi_cli 3 --fast)
add_test(NAME cli_rejects_leading_space COMMAND hanoi_cli " 3")
add_test(NAME cli_rejects_plus_sign COMMAND hanoi_cli "+3")
set_tests_properties(cli_rejects_empty cli_rejects_no_argument cli_rejects_bad_flag
                     cli_rejects_leading_space cli_rejects_plus_sign
                     PROPERTIES WILL_FAIL TRUE)
