cmake_minimum_required(VERSION 3.20)
project(defensive_programming_cpp LANGUAGES CXX)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  set(WARNINGS /W4 /WX /permissive- /EHsc)
else()
  set(WARNINGS -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()

add_library(defensive STATIC src/defensive.cpp)
target_include_directories(defensive PUBLIC src)
target_compile_options(defensive PRIVATE ${WARNINGS})

add_executable(people src/people.cpp)
add_executable(test_defensive tests/test_defensive.cpp)
add_executable(pitfall_vector_index pitfalls/vector_index.cpp)
foreach(t people test_defensive pitfall_vector_index)
  target_link_libraries(${t} PRIVATE defensive)
  target_compile_options(${t} PRIVATE ${WARNINGS})
endforeach()

enable_testing()
set(TEMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/tmp)
file(MAKE_DIRECTORY ${TEMP_DIR})
add_test(NAME unit_tests COMMAND test_defensive ${CMAKE_CURRENT_SOURCE_DIR}/data ${TEMP_DIR})

function(output_test name prog expected)
  add_test(NAME ${name} COMMAND ${CMAKE_COMMAND}
    -DPROG=$<TARGET_FILE:${prog}> -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${expected}
    "-DARGS=${ARGN}" -DWORKDIR=${CMAKE_CURRENT_SOURCE_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endfunction()
# people.txt is the same expected file as in the C example: both programs
# must print exactly the same report.
output_test(people_output people people.txt data/people.txt)
output_test(vector_index_output pitfall_vector_index vector-index.txt)

add_test(NAME ignored_status_fails COMMAND ${CMAKE_COMMAND}
  -DCXX=${CMAKE_CXX_COMPILER} -DMSVC=${MSVC} -DSTD=${CMAKE_CXX_STANDARD}
  -DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/compile_fail/ignored_status.cpp -DOBJ=${TEMP_DIR}/ignored.obj
  "-DEXPECT=nodiscard|C4834"
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/expect_build_failure.cmake)
