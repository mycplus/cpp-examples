cmake_minimum_required(VERSION 3.16)
project(smart_pointers CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

set(EXAMPLES
    raw_vs_unique unique_basics sizes file_handle allocations
    shared_basics cycle_leak cycle_fixed shared_from_this long_list threads)

function(add_example name)
    add_executable(${name} src/${name}.cpp)
    target_link_libraries(${name} PRIVATE Threads::Threads)
    if(MSVC)
        target_compile_options(${name} PRIVATE /W4 /WX /permissive-)
        target_compile_definitions(${name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(${name} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endfunction()

foreach(example IN LISTS EXAMPLES)
    add_example(${example})
endforeach()

# std::atomic<std::shared_ptr<T>> is a C++20 library feature.
add_example(atomic_shared)
target_compile_features(atomic_shared PRIVATE cxx_std_20)

# Compile-only: the parameter-passing comparison from the article (no main).
add_library(abi_listing OBJECT tests/asm/abi.cpp)
if(MSVC)
    target_compile_options(abi_listing PRIVATE /W4 /WX /permissive-)
else()
    target_compile_options(abi_listing PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
