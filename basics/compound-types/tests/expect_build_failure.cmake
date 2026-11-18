# Build TARGET in BUILD_DIR and pass only if the build FAILS with a compiler
# message matching PATTERN. A build that fails for an unrelated reason (a
# missing file, a typo) does not match the pattern and fails the test.
set(cfg)
if(CONFIG)
  set(cfg --config ${CONFIG})
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR} --target ${TARGET} ${cfg}
                OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
if(rc EQUAL 0)
  message(FATAL_ERROR "${TARGET} compiled, but it must not")
endif()
if(NOT "${out}${err}" MATCHES "${PATTERN}")
  message(FATAL_ERROR "${TARGET} failed to build, but without a message matching '${PATTERN}':\n${out}${err}")
endif()
message(STATUS "${TARGET} was rejected as expected")
