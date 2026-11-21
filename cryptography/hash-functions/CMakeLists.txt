cmake_minimum_required(VERSION 3.16)
project(hash_functions CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/W4 /WX /permissive-)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(include)
add_executable(digest_demo src/digest_demo.cpp)
add_executable(test_vectors tests/test_vectors.cpp)

enable_testing()
set(VECTORS ${CMAKE_CURRENT_SOURCE_DIR}/tests/vectors)
function(expect_output name expected prog)
  add_test(NAME ${name} COMMAND ${CMAKE_COMMAND}
    -DPROG=${prog} "-DARGS=${ARGN}"
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${expected}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endfunction()
expect_output(demo_output digest_demo.txt $<TARGET_FILE:digest_demo>)
expect_output(vectors test_vectors.txt $<TARGET_FILE:test_vectors> ${VECTORS})

# The library to use in real code: OpenSSL, when it is installed.
find_package(OpenSSL QUIET)
if(OPENSSL_FOUND)
  add_executable(openssl_digest src/openssl_digest.cpp)
  target_link_libraries(openssl_digest PRIVATE OpenSSL::Crypto)
  expect_output(openssl_digest openssl_digest.txt $<TARGET_FILE:openssl_digest>)
endif()
