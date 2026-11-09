cmake_minimum_required(VERSION 3.16)
project(tone_mapping CXX)

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

add_executable(tone_mapping  src/tone_mapping.cpp)
add_executable(test_tonemap  tests/test_tonemap.cpp)

enable_testing()
add_test(NAME tonemap_unit COMMAND test_tonemap)
add_test(NAME output_table
         COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:tone_mapping>
                 -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/tone_mapping.txt
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)

# If glslangValidator is installed, compile the GLSL shader to SPIR-V as a test.
find_program(GLSLANG_VALIDATOR glslangValidator)
if(GLSLANG_VALIDATOR)
  add_test(NAME shader_spirv
           COMMAND ${GLSLANG_VALIDATOR} -V ${CMAKE_CURRENT_SOURCE_DIR}/shaders/tonemap.frag
                   -o ${CMAKE_CURRENT_BINARY_DIR}/tonemap.frag.spv)
endif()
