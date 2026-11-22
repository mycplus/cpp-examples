cmake_minimum_required(VERSION 3.16)
project(vcruntime_deps CXX)

if(NOT MSVC)
  message(FATAL_ERROR "This example checks what MSVC links against; build it with Visual Studio. "
                      "run_all.sh only compile-checks the sources with MinGW-w64.")
endif()

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(/W4 /WX /permissive- /EHsc)

# x64, X86 or ARM64: the architecture this build produces.
set(ARCH ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID})
message(STATUS "Target architecture: ${ARCH}")

# Four builds of the same kind of program:
add_executable(eh_demo src/eh_demo.cpp)                # default: dynamic runtime (/MD)
add_executable(eh_demo_nofh4 src/eh_demo.cpp)          # x64 only: old exception tables (/d2FH4-)
add_executable(eh_demo_static src/eh_demo.cpp)         # static runtime (/MT): no runtime DLLs
add_executable(hello src/hello.cpp)                    # no exceptions at all
if(ARCH STREQUAL "x64")
  target_compile_options(eh_demo_nofh4 PRIVATE /d2FH4-)
  target_link_options(eh_demo_nofh4 PRIVATE /d2:-FH4-)
endif()
set_property(TARGET eh_demo_static PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

# The checker uses the static runtime so it starts even with no runtime installed.
add_executable(runtime_check src/runtime_check.cpp)
set_property(TARGET runtime_check PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
target_link_libraries(runtime_check PRIVATE version)

enable_testing()

# dumpbin lives next to cl.exe.
get_filename_component(MSVC_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(DUMPBIN dumpbin HINTS "${MSVC_BIN}" REQUIRED)

# deps_test(<target> <regex that must appear> [<regex that must not appear>])
function(deps_test target must)
  add_test(NAME ${target}_imports COMMAND "${DUMPBIN}" /dependents $<TARGET_FILE:${target}>)
  set_tests_properties(${target}_imports PROPERTIES PASS_REGULAR_EXPRESSION "${must}")
  if(ARGC GREATER 2)
    set_tests_properties(${target}_imports PROPERTIES FAIL_REGULAR_EXPRESSION "${ARGV2}")
  endif()
endfunction()

if(ARCH STREQUAL "x64")
  deps_test(eh_demo        "VCRUNTIME140_1\\.dll")
  deps_test(eh_demo_nofh4  "VCRUNTIME140\\.dll"  "VCRUNTIME140_1\\.dll")
  deps_test(hello          "VCRUNTIME140\\.dll"  "VCRUNTIME140_1\\.dll")
elseif(ARCH STREQUAL "X86" OR ARCH STREQUAL "ARM64")
  # Only x64 builds import VCRUNTIME140_1.dll (ARM64 confirmed with MSVC 14.52).
  deps_test(eh_demo        "VCRUNTIME140\\.dll"  "VCRUNTIME140_1\\.dll")
  deps_test(hello          "VCRUNTIME140\\.dll"  "VCRUNTIME140_1\\.dll")
else()
  # Anything else: record what the linker chose, without asserting it.
  add_test(NAME eh_demo_imports_info COMMAND "${DUMPBIN}" /dependents $<TARGET_FILE:eh_demo>)
endif()
deps_test(eh_demo_static "KERNEL32\\.dll" "VCRUNTIME140")

# Run the program too, when this machine can run the target architecture.
if(NOT ARCH STREQUAL "ARM64" OR CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "ARM64")
  add_test(NAME eh_demo_output COMMAND ${CMAKE_COMMAND}
    -DPROG=$<TARGET_FILE:eh_demo>
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/eh_demo.txt
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endif()
