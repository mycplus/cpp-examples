cmake_minimum_required(VERSION 3.16)
project(brick_game_cpp CXX)

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
add_library(brick_core STATIC src/game.cpp src/screen.cpp)

add_executable(brick    src/main.cpp src/terminal.cpp)
add_executable(autoplay src/autoplay.cpp)
target_link_libraries(brick brick_core)
target_link_libraries(autoplay brick_core)

add_executable(test_game   tests/test_game.cpp)
add_executable(test_screen tests/test_screen.cpp)
target_link_libraries(test_game brick_core)
target_link_libraries(test_screen brick_core)

enable_testing()
add_test(NAME rules  COMMAND test_game)
add_test(NAME screen COMMAND test_screen)
# Byte for byte the same output as the C version of this game
add_test(NAME output_autoplay
         COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:autoplay>
                 -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/autoplay.txt
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
