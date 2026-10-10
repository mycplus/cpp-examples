cmake_minimum_required(VERSION 3.16)
project(cpp_multiple_inheritance CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

foreach(prog interfaces name_clash diamond virtual_base base_order)
    add_executable(${prog} src/${prog}.cpp)
    if(MSVC)
        target_compile_options(${prog} PRIVATE /W4 /WX /EHsc)
    else()
        target_compile_options(${prog} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endforeach()

# base_order.cpp demonstrates the -Wreorder warning, so that one warning is
# allowed for it on GCC and Clang.
if(NOT MSVC)
    target_compile_options(base_order PRIVATE -Wno-reorder)
endif()
