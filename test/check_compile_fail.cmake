if(NOT DEFINED BUILD_DIR OR NOT DEFINED TARGET_NAME OR NOT DEFINED EXPECTED_TEXT)
  message(FATAL_ERROR "BUILD_DIR, TARGET_NAME, and EXPECTED_TEXT are required.")
endif()

if(COMPILE_COMMANDS)
  file(READ "${COMPILE_COMMANDS}" commands)
  string(JSON count LENGTH "${commands}")
  math(EXPR last "${count} - 1")
  set(compile_command)
  foreach(index RANGE ${last})
    string(JSON command GET "${commands}" ${index} command)
    string(REPLACE "\\" "/" normalized "${command}")
    string(FIND "${normalized}" "CMakeFiles/${TARGET_NAME}.dir/" position)
    if(NOT position EQUAL -1)
      string(JSON directory GET "${commands}" ${index} directory)
      if(CMAKE_HOST_WIN32)
        # The trailing backslash in MSVC /Fd<dir>\ would join the next argument.
        string(REGEX REPLACE "(/Fd[^ \"]*)\\\\ " "\\1/ " command "${command}")
      endif()
      separate_arguments(compile_command NATIVE_COMMAND "${command}")
      break()
    endif()
  endforeach()
  if(NOT compile_command)
    message(FATAL_ERROR "No compiler command found for '${TARGET_NAME}'.")
  endif()

  # Each target has its own object and PDB paths. CMake's build tool normally creates them.
  set(object)
  set(next_is_object FALSE)
  foreach(argument IN LISTS compile_command)
    if(next_is_object)
      set(object "${argument}")
      break()
    elseif(argument STREQUAL "-o")
      set(next_is_object TRUE)
    elseif(argument MATCHES "^/Fo(.+)$")
      set(object "${CMAKE_MATCH_1}")
      break()
    endif()
  endforeach()
  if(NOT object)
    message(FATAL_ERROR "No object path found for '${TARGET_NAME}'.")
  endif()
  get_filename_component(object "${object}" ABSOLUTE BASE_DIR "${directory}")
  get_filename_component(object_dir "${object}" DIRECTORY)
  file(MAKE_DIRECTORY "${object_dir}")
else()
  set(compile_command "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target "${TARGET_NAME}")
  set(directory "${BUILD_DIR}")
  if(DEFINED CONFIG AND NOT CONFIG STREQUAL "")
    list(APPEND compile_command --config "${CONFIG}")
  endif()
endif()

execute_process(
  COMMAND ${compile_command}
  WORKING_DIRECTORY "${directory}"
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
