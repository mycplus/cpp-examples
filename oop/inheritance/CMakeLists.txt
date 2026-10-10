cmake_minimum_required(VERSION 3.16)
project(cpp_inheritance CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

foreach(prog basics access order inheriting_ctors hiding final_and_override
             array_pitfall virtual_dtor)
    add_executable(${prog} src/${prog}.cpp)
    if(MSVC)
        target_compile_options(${prog} PRIVATE /W4 /WX /EHsc)
    else()
        target_compile_options(${prog} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endforeach()
