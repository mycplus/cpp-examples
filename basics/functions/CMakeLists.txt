cmake_minimum_required(VERSION 3.16)
project(cpp_functions CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# dangling_return.cpp is not built here: it exists to show a warning and is
# undefined behavior when run (see tests/run_tests.sh and tests/sanitizers.sh).
foreach(prog declare_define passing defaults overloading eval_order variadic
             returning function_values constexpr_fn)
    add_executable(${prog} src/${prog}.cpp)
    if(MSVC)
        target_compile_options(${prog} PRIVATE /W4 /WX /EHsc)
    else()
        target_compile_options(${prog} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endforeach()
