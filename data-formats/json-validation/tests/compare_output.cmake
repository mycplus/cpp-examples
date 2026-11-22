# Runs PROG with the case files in ARGS (a list, relative to CASES) and compares
# its output with EXPECTED. MODE=verdicts keeps only "<file>: valid|invalid".
execute_process(COMMAND ${PROG} ${ARGS} WORKING_DIRECTORY ${CASES}
  OUTPUT_VARIABLE out RESULT_VARIABLE rc)
if(NOT rc MATCHES "^[01]$")
  message(FATAL_ERROR "${PROG} failed: ${rc}")
endif()
string(REPLACE "\r\n" "\n" out "${out}")
if(MODE STREQUAL "verdicts")
  # Brackets, backslashes and semicolons would confuse CMake's list splitting.
  string(REGEX REPLACE "[][;\\]" "_" text "${out}")
  string(REPLACE "\n" ";" lines "${text}")
  set(out "")
  foreach(line IN LISTS lines)
    if(line MATCHES "^([^ :]+\\.json): (valid|invalid)")
      string(APPEND out "${CMAKE_MATCH_1}: ${CMAKE_MATCH_2}\n")
    endif()
  endforeach()
endif()
file(READ ${EXPECTED} expected)
string(REPLACE "\r\n" "\n" expected "${expected}")
if(NOT out STREQUAL expected)
  message(FATAL_ERROR "Output differs from ${EXPECTED}\n--- actual ---\n${out}")
endif()
