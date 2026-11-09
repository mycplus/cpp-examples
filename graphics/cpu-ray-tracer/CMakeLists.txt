cmake_minimum_required(VERSION 3.16)
project(cpu_ray_tracer CXX)

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

add_executable(ray_tracer     src/ray_tracer.cpp)
add_executable(test_raytracer tests/test_raytracer.cpp)

enable_testing()
add_test(NAME raytracer_unit COMMAND test_raytracer)
# Renders a small image and compares the printed summary with the expected file.
add_test(NAME output_render
         COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:ray_tracer>
                 "-DARGS=test_render.ppm;160;90"
                 -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/render_160x90.txt
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
