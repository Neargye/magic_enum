if(NOT DEFINED BUILD_DIR OR NOT DEFINED STAGE_DIR)
  message(FATAL_ERROR "BUILD_DIR and STAGE_DIR are required")
endif()

file(REMOVE_RECURSE "${STAGE_DIR}")
if(CONSUMER_BUILD_DIR)
  file(REMOVE_RECURSE "${CONSUMER_BUILD_DIR}")
endif()

set(_install_command "${CMAKE_COMMAND}" --install "${BUILD_DIR}" --prefix "${STAGE_DIR}")
if(CONFIG)
  list(APPEND _install_command --config "${CONFIG}")
endif()

execute_process(COMMAND ${_install_command} COMMAND_ERROR_IS_FATAL ANY)
