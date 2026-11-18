cmake_minimum_required(VERSION 3.16)
project(named_pipes CXX)

if(NOT WIN32)
  message(FATAL_ERROR "These examples use the Win32 named pipe API and build only on Windows. "
                      "run_all.sh cross-compiles them with MinGW-w64 instead.")
endif()

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/W4 /WX /permissive- /utf-8)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()
add_compile_definitions(UNICODE _UNICODE)

# Put every .exe in one folder, so pipe_test can start the others.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
foreach(config IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER ${config} CONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR}/bin)
endforeach()

enable_testing()

foreach(p IN ITEMS pipe_server pipe_client thread_server overlapped_server)
  add_executable(${p} src/${p}.cpp)
endforeach()
add_executable(pipe_test tests/pipe_test.cpp)
add_dependencies(pipe_test pipe_server pipe_client thread_server overlapped_server)

# Each scenario starts servers and clients and must print exactly
# tests/expected/<scenario>.txt.
foreach(s IN ITEMS simple thread overlapped long-simple long-overlapped duplicate-name no-server)
  add_test(NAME pipes_${s} COMMAND ${CMAKE_COMMAND}
    -DPROG=$<TARGET_FILE:pipe_test>
    "-DARGS=${CMAKE_BINARY_DIR}/bin;${s}"
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${s}.txt
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
  set_tests_properties(pipes_${s} PROPERTIES TIMEOUT 60)
endforeach()
