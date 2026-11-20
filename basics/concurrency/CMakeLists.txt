cmake_minimum_required(VERSION 3.16)
project(concurrency CXX)

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

# std::thread needs the platform thread library (-pthread on Linux).
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

enable_testing()

function(output_test name expected)
  add_test(NAME ${name} COMMAND ${CMAKE_COMMAND}
    -DPROG=$<TARGET_FILE:${ARGV2}> "-DARGS=${ARGV3}"
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${expected}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
  # A concurrency bug often shows up as a hang, not a wrong answer.
  set_tests_properties(${name} PROPERTIES TIMEOUT 60)
endfunction()

foreach(p IN ITEMS threads_basics counter producer_consumer futures transfers)
  add_executable(${p} src/${p}.cpp)
endforeach()

# The C++20 synchronization primitives need C++20, whatever the default is.
add_executable(cpp20_sync src/cpp20_sync.cpp)
target_compile_features(cpp20_sync PRIVATE cxx_std_20)

output_test(threads_basics_output threads_basics.txt threads_basics "")
output_test(counter_mutex_output counter_mutex.txt counter mutex)
output_test(counter_atomic_output counter_atomic.txt counter atomic)
output_test(producer_consumer_output producer_consumer.txt producer_consumer "")
output_test(futures_output futures.txt futures "")
output_test(transfers_output transfers.txt transfers "")
output_test(cpp20_sync_output cpp20_sync.txt cpp20_sync "")
