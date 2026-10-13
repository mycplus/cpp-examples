cmake_minimum_required(VERSION 3.16)
project(knights_tour CXX)

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

add_executable(knights_tour src/knights_tour.cpp)   # the article's program
add_executable(tour_stats src/tour_stats.cpp)
add_executable(test_tour tests/test_tour.cpp)

enable_testing()
add_test(NAME test_tour COMMAND test_tour)

function(expect_output name expected prog)
  add_test(NAME ${name} COMMAND ${CMAKE_COMMAND}
    -DPROG=${prog} "-DARGS=${ARGN}"
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${expected}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endfunction()

expect_output(default_output knights_tour_8.txt $<TARGET_FILE:knights_tour>)
expect_output(start_1_1_output knights_tour_8_1_1.txt $<TARGET_FILE:knights_tour> "8,1,1")
expect_output(board_5_output knights_tour_5.txt $<TARGET_FILE:knights_tour> 5)
expect_output(tour_stats_output tour_stats.txt $<TARGET_FILE:tour_stats>)

# Rejected arguments exit with status 2; boards with no tour exit with 1.
add_test(NAME rejects_size_0 COMMAND knights_tour 0)
add_test(NAME rejects_size_31 COMMAND knights_tour 31)
add_test(NAME rejects_letters COMMAND knights_tour abc)
add_test(NAME rejects_row_8 COMMAND knights_tour 8 8 0)
add_test(NAME rejects_negative_col COMMAND knights_tour 8 0 -1)
add_test(NAME rejects_missing_col COMMAND knights_tour 8 1)
add_test(NAME rejects_extra_arg COMMAND knights_tour 8 1 1 1)
add_test(NAME rejects_leading_space COMMAND knights_tour " 8")
add_test(NAME rejects_plus_sign COMMAND knights_tour "+8")
add_test(NAME no_tour_on_4 COMMAND knights_tour 4)
add_test(NAME no_tour_from_minority_color COMMAND knights_tour 5 0 1)
set_tests_properties(rejects_size_0 rejects_size_31 rejects_letters rejects_row_8
                     rejects_negative_col rejects_missing_col rejects_extra_arg
                     rejects_leading_space rejects_plus_sign no_tour_on_4
                     no_tour_from_minority_color PROPERTIES WILL_FAIL TRUE)
