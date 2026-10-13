cmake_minimum_required(VERSION 3.16)
project(car_race_game_cpp CXX)

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
add_library(race_core STATIC src/game.cpp src/road.cpp src/screen.cpp)

add_executable(race       src/main.cpp src/terminal.cpp)
add_executable(autoplay   src/autoplay.cpp)
add_executable(road_stats src/road_stats.cpp)
add_executable(random_check src/random_check.cpp)
target_link_libraries(race race_core)
target_link_libraries(autoplay race_core)
target_link_libraries(road_stats race_core)

add_executable(test_race   tests/test_race.cpp)
add_executable(test_screen tests/test_screen.cpp)
target_link_libraries(test_race race_core)
target_link_libraries(test_screen race_core)

enable_testing()
add_test(NAME rules  COMMAND test_race)
add_test(NAME screen COMMAND test_screen)
add_test(NAME random_check COMMAND random_check)
set_tests_properties(random_check PROPERTIES PASS_REGULAR_EXPRESSION "10000th mt19937 value: 4123659995")
add_test(NAME output_autoplay
         COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:autoplay>
                 -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/autoplay.txt
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
add_test(NAME output_road_stats
         COMMAND ${CMAKE_COMMAND} -DPROG=$<TARGET_FILE:road_stats>
                 -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/road_stats.txt
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
