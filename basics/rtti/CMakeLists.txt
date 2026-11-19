cmake_minimum_required(VERSION 3.16)
project(rtti CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/W4 /WX /permissive- /EHsc)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

enable_testing()

# The article's programs: each must print exactly tests/expected/<name>.txt.
foreach(p IN ITEMS typeid_basics dynamic_cast_basics construction alternatives)
  add_executable(${p} src/${p}.cpp)
  add_test(NAME ${p}_output COMMAND ${CMAKE_COMMAND}
    -DPROG=$<TARGET_FILE:${p}>
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${p}.txt
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endforeach()

# Programs that must NOT compile. They are left out of the normal build; each
# test builds one and passes only if the build fails with a matching message.
function(expect_build_failure name pattern)
  add_executable(${name} EXCLUDE_FROM_ALL tests/compile_fail/${name}.cpp)
  add_test(NAME ${name}_must_not_compile COMMAND ${CMAKE_COMMAND}
    -DBUILD_DIR=${CMAKE_BINARY_DIR} -DTARGET=${name} -DCONFIG=$<CONFIG>
    "-DPATTERN=${pattern}"
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/expect_build_failure.cmake)
endfunction()

# GCC, Clang and MSVC word the errors differently; each pattern accepts all three.
expect_build_failure(not_polymorphic "not polymorphic|not a polymorphic type")
expect_build_failure(from_void "not a pointer to class|is not a class type|invalid expression type for dynamic_cast")

# The benchmark is built but not tested: its numbers depend on the machine.
add_executable(cast_cost bench/cast_cost.cpp)
