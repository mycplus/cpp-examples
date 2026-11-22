cmake_minimum_required(VERSION 3.20)
project(json_validation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# nlohmann/json 3.12.0, pinned by hash.
include(FetchContent)
FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
  URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(json)

foreach(prog validate_json validate_user edge_cases)
  add_executable(${prog} src/${prog}.cpp)
  target_link_libraries(${prog} PRIVATE nlohmann_json::nlohmann_json)
  if(MSVC)
    target_compile_options(${prog} PRIVATE /W4 /WX /permissive- /utf-8)
  else()
    target_compile_options(${prog} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Werror)
  endif()
endforeach()

enable_testing()
set(CASES ${CMAKE_CURRENT_SOURCE_DIR}/cases)
set(EXPECTED ${CMAKE_CURRENT_SOURCE_DIR}/tests/expected)

# Each test runs a program over a list of case files (relative to cases/) and
# compares its output with an expected file. MODE=verdicts compares only
# "file: valid" / "file: invalid", the part all three languages share.
function(output_test name prog expected mode)
  add_test(NAME ${name} COMMAND ${CMAKE_COMMAND}
    -DPROG=$<TARGET_FILE:${prog}> -DCASES=${CASES} -DEXPECTED=${EXPECTED}/${expected}
    -DMODE=${mode} "-DARGS=${ARGN}"
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endfunction()

file(GLOB valid_files RELATIVE ${CASES} ${CASES}/valid/*.json)
file(GLOB invalid_files RELATIVE ${CASES} ${CASES}/invalid/*.json)
file(GLOB schema_docs RELATIVE ${CASES} ${CASES}/schema/user-*.json)
file(GLOB edge_files RELATIVE ${CASES} ${CASES}/edge/*.json)
list(SORT valid_files)
list(SORT invalid_files)
list(SORT schema_docs)
list(SORT edge_files)

output_test(syntax_verdicts validate_json verdicts.txt verdicts ${valid_files} ${invalid_files})
output_test(syntax_messages validate_json validate_json.txt full ${valid_files} ${invalid_files})
output_test(user_verdicts validate_user user-verdicts.txt verdicts ${schema_docs})
output_test(user_messages validate_user validate_user.txt full ${schema_docs})
output_test(edge_cases edge_cases edge_cases.txt full ${edge_files})
