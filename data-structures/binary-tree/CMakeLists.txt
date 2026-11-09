cmake_minimum_required(VERSION 3.16)
project(binary_tree_cpp CXX)

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

add_executable(bst_demo   src/bst_demo.cpp)
add_executable(set_vs_bst src/set_vs_bst.cpp)
add_executable(less_equal pitfalls/less_equal.cpp)
add_executable(test_bst   tests/test_bst.cpp)

enable_testing()
add_test(NAME bst_unit COMMAND test_bst)
# The article's printed output, checked byte for byte
add_test(NAME output_bst_demo
         COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:bst_demo>
                 -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/bst_demo.txt
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
# The <= comparator pitfall: 40 is stored twice and then cannot be found
add_test(NAME pitfall_less_equal COMMAND less_equal)
set_tests_properties(pitfall_less_equal PROPERTIES
                     PASS_REGULAR_EXPRESSION "contains 40: no")

# Destroying a 20,000-node chain on a 64 KiB thread stack (POSIX threads)
if(UNIX)
  find_package(Threads REQUIRED)
  add_executable(test_deep_destroy tests/test_deep_destroy.cpp)
  target_link_libraries(test_deep_destroy PRIVATE Threads::Threads)
  add_test(NAME deep_destroy COMMAND test_deep_destroy)
endif()
