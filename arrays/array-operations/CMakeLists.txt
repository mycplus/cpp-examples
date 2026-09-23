cmake_minimum_required(VERSION 3.16)
project(array_operations CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(array-demo src/array-demo.cpp)
target_include_directories(array-demo PRIVATE include)

enable_testing()
add_executable(test_array_ops tests/test_array_ops.cpp)
target_include_directories(test_array_ops PRIVATE include)
add_test(NAME array_ops_tests COMMAND test_array_ops)
