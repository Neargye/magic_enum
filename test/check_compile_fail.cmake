if(NOT DEFINED BUILD_DIR OR NOT DEFINED TARGET_NAME OR NOT DEFINED EXPECTED_TEXT)
  message(FATAL_ERROR "BUILD_DIR, TARGET_NAME, and EXPECTED_TEXT are required.")
endif()

set(build_command "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target "${TARGET_NAME}")
if(DEFINED CONFIG AND NOT CONFIG STREQUAL "")
  list(APPEND build_command --config "${CONFIG}")
endif()

execute_process(
  COMMAND ${build_command}
  RESULT_VARIABLE build_result
  OUTPUT_VARIABLE build_stdout
  ERROR_VARIABLE build_stderr
)

set(build_output "${build_stdout}\n${build_stderr}")

if(build_result EQUAL 0)
  message(FATAL_ERROR
    "Compile-fail target '${TARGET_NAME}' unexpectedly compiled successfully.\n"
    "${build_output}"
  )
endif()

string(FIND "${build_output}" "${EXPECTED_TEXT}" diagnostic_position)
if(diagnostic_position EQUAL -1)
  message(FATAL_ERROR
    "Compile-fail target '${TARGET_NAME}' failed without expected diagnostic:\n"
    "  ${EXPECTED_TEXT}\n"
    "Compiler output:\n${build_output}"
  )
endif()

message(STATUS
  "Compile-fail target '${TARGET_NAME}' produced expected diagnostic: "
  "${EXPECTED_TEXT}"
)
