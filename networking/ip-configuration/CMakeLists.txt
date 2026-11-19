cmake_minimum_required(VERSION 3.16)
project(ip_configuration CXX)

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

# The address code is portable; the interface code has one file per platform.
if(WIN32)
  add_library(netconfig STATIC src/ip_text.cpp src/interfaces_windows.cpp)
  target_link_libraries(netconfig PUBLIC iphlpapi ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_library(netconfig STATIC src/ip_text.cpp src/interfaces_linux.cpp)
else()
  message(FATAL_ERROR "Only Windows and Linux are supported")
endif()

add_executable(ipaddr src/ipaddr.cpp)
target_link_libraries(ipaddr PRIVATE netconfig)

enable_testing()

foreach(t IN ITEMS ip_text_test list_test change_test)
  add_executable(${t} tests/${t}.cpp)
  target_link_libraries(${t} PRIVATE netconfig)
  add_test(NAME ${t} COMMAND ${t})
endforeach()
# change_test adds and removes two test addresses on the loopback interface.
# Without administrator rights (Windows) or root (Linux) it exits with 77.
set_tests_properties(change_test PROPERTIES SKIP_RETURN_CODE 77)

# The command-line tool: messages for bad input, and the loopback address in the list.
add_test(NAME ipaddr_usage COMMAND ipaddr)
set_tests_properties(ipaddr_usage PROPERTIES PASS_REGULAR_EXPRESSION "usage: ipaddr list")
add_test(NAME ipaddr_bad_address COMMAND ipaddr add 1 300.1.1.1/24)
set_tests_properties(ipaddr_bad_address PROPERTIES PASS_REGULAR_EXPRESSION "\"300\\.1\\.1\\.1/24\" is not a valid address")
add_test(NAME ipaddr_no_interface COMMAND ipaddr add no-such-interface 198.18.77.77/32)
set_tests_properties(ipaddr_no_interface PROPERTIES PASS_REGULAR_EXPRESSION "no interface named or numbered \"no-such-interface\"")
add_test(NAME ipaddr_list COMMAND ipaddr list)
set_tests_properties(ipaddr_list PROPERTIES PASS_REGULAR_EXPRESSION "127\\.0\\.0\\.1/8  \\(mask 255\\.0\\.0\\.0\\)")
