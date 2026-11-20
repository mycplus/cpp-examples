# Run PROG with ARGS and compare its stdout with EXPECTED. Carriage returns are
# removed first: on Windows, text-mode output writes \r\n line endings.
execute_process(COMMAND ${PROG} ${ARGS} OUTPUT_VARIABLE out RESULT_VARIABLE rc ENCODING UTF-8)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "${PROG} exited with ${rc}:\n${out}")
endif()
file(READ ${EXPECTED} want)
string(REPLACE "\r" "" out "${out}")
string(REPLACE "\r" "" want "${want}")
if(NOT out STREQUAL want)
  message(FATAL_ERROR "output of ${PROG} differs from ${EXPECTED}:\n${out}")
endif()
