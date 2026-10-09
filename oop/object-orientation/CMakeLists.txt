cmake_minimum_required(VERSION 3.16)
project(object_orientation CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

foreach(prog library slicing operators variant_shapes missing_override)
    add_executable(${prog} src/${prog}.cpp)
    if(MSVC)
        target_compile_options(${prog} PRIVATE /W4 /WX /EHsc)
    else()
        target_compile_options(${prog} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endforeach()

# missing_override.cpp demonstrates the -Woverloaded-virtual warning, so that
# one warning is allowed for it on GCC and Clang.
if(NOT MSVC)
    target_compile_options(missing_override PRIVATE -Wno-overloaded-virtual)
endif()
