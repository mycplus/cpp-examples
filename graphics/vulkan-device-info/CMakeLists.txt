cmake_minimum_required(VERSION 3.16)
project(vulkan_device_info CXX)

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

find_package(Vulkan REQUIRED)

add_executable(vulkan_device_info src/vulkan_device_info.cpp)
target_link_libraries(vulkan_device_info PRIVATE Vulkan::Vulkan)

enable_testing()
# Needs a Vulkan driver at run time. CI uses Mesa's lavapipe, a software
# (CPU) Vulkan driver, so the output must name llvmpipe.
add_test(NAME lists_a_device COMMAND vulkan_device_info)
set_tests_properties(lists_a_device PROPERTIES
  PASS_REGULAR_EXPRESSION "Vulkan devices: [1-9].*queue family 0: [0-9]+ queue\\(s\\) graphics compute")
