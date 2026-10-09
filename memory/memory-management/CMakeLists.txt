cmake_minimum_required(VERSION 3.16)
project(memory_management CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Correct examples: built with warnings as errors.
foreach(prog new_delete ownership exception_safety)
    add_executable(${prog} src/${prog}.cpp)
    if(MSVC)
        target_compile_options(${prog} PRIVATE /W4 /WX /EHsc)
    else()
        target_compile_options(${prog} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endforeach()

# throwing_destructor exists to show the warning its compiler gives, so that
# warning is disabled (two on MSVC: C4297 and C4722) and every other warning
# is still an error.
add_executable(throwing_destructor src/throwing_destructor.cpp)
if(MSVC)
    target_compile_options(throwing_destructor PRIVATE /W4 /WX /EHsc /wd4297 /wd4722)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(throwing_destructor PRIVATE -Wall -Wextra -pedantic -Werror -Wno-exceptions)
else()
    target_compile_options(throwing_destructor PRIVATE -Wall -Wextra -pedantic -Werror -Wno-terminate)
endif()

# Deliberately broken programs: built without -Werror, because their warnings
# are part of what the article shows. Run them under a sanitizer or Valgrind.
foreach(prog shallow_copy dangling_temporary memory_errors)
    add_executable(${prog} src/${prog}.cpp)
endforeach()
