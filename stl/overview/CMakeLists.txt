cmake_minimum_required(VERSION 3.16)
project(stl_overview CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(add_example name)
    add_executable(${name} src/${name}.cpp)
    if(MSVC)
        target_compile_options(${name} PRIVATE /W4 /WX /permissive- /EHsc)
    else()
        target_compile_options(${name} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endfunction()

foreach(example first_look containers adaptors lookup_cost stable_positions
                map_lookup function_objects stream_iterators algorithm_counts
                modern_adapters)
    add_example(${example})
endforeach()

# C++20: std::erase (erase_remove.cpp guards it with __cpp_lib_erase_if) and ranges.
foreach(example erase_remove ranges_pipeline)
    add_example(${example})
    set_target_properties(${example} PROPERTIES CXX_STANDARD 20)
endforeach()
