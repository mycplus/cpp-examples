cmake_minimum_required(VERSION 3.16)
project(stack_cpp CXX)

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

include_directories(include)

add_executable(stack_demo    src/stack_demo.cpp)
add_executable(linked_demo   src/linked_demo.cpp)
add_executable(vector_growth src/vector_growth.cpp)
add_executable(test_stacks   tests/test_stacks.cpp)

enable_testing()
add_test(NAME stacks_unit COMMAND test_stacks)
# The article's printed outputs, checked byte for byte
foreach(prog stack_demo linked_demo)
  add_test(NAME output_${prog}
           COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:${prog}>
                   -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${prog}.txt
                   -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endforeach()
