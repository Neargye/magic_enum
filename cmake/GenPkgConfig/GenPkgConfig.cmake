#[=======================================================================[.rst:

GenPkgConfig
------------

This module generates and installs pkg-config files from CMake targets.

Unlicense
^^^^^^^^^

This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this software dedicate any and all copyright interest in the software to the public domain. We make this dedication for the benefit of the public at large and to the detriment of our heirs and successors. We intend this dedication to be an overt act of relinquishment in perpetuity of all present and future rights to this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
For more information, please refer to <https://unlicense.org/>.

Functions
^^^^^^^^^

.. command:: configure_pkg_config_file

  Requires CMake 3.22 or newer.

  Generates and installs a pkg-config file for an interface, object, static, or shared library target.

  ::

    configure_pkg_config_file(<target>
        NAME <package name>
        VERSION <package version>
        DESCRIPTION <package description>
        URL <package homepage>
        COMPONENT <install component>
        INSTALL_LIB_DIR <library install directory>
        INSTALL_INCLUDE_DIR <include install directory>
        REQUIRES <required pkg-config packages> ...
        CONFLICTS <conflicting pkg-config packages> ...
    )

  ``VERSION`` and ``DESCRIPTION`` fall back to project or CPack metadata. Install directories must be relative. Compiled targets must be installed into ``INSTALL_LIB_DIR`` by the caller; object files are installed by this module.

#]=======================================================================]

include(GNUInstallDirs)

function(configure_pkg_config_file TARGET)
  cmake_parse_arguments(PARSE_ARGV 1 ARG
    ""
    "NAME;VERSION;DESCRIPTION;URL;COMPONENT;INSTALL_LIB_DIR;INSTALL_INCLUDE_DIR"
    "REQUIRES;CONFLICTS"
  )

  if(ARG_KEYWORDS_MISSING_VALUES)
    list(JOIN ARG_KEYWORDS_MISSING_VALUES ", " _MISSING_ARGUMENTS)
    message(FATAL_ERROR
      "configure_pkg_config_file missing values for: ${_MISSING_ARGUMENTS}"
    )
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    list(JOIN ARG_UNPARSED_ARGUMENTS ", " _UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "configure_pkg_config_file unknown arguments: ${_UNPARSED_ARGUMENTS}"
    )
  endif()

  configure_pkg_config_file_vars(
    "${TARGET}"
    "${ARG_NAME}"
    "${ARG_INSTALL_LIB_DIR}"
    "${ARG_INSTALL_INCLUDE_DIR}"
    "${ARG_COMPONENT}"
    "${ARG_DESCRIPTION}"
    "${ARG_URL}"
    "${ARG_VERSION}"
    "${ARG_REQUIRES}"
    "${ARG_CONFLICTS}"
  )
endfunction()

function(configure_pkg_config_file_vars
  TARGET
  _NAME
  _INSTALL_LIB_DIR
  _INSTALL_INCLUDE_DIR
  _COMPONENT
  _DESCRIPTION
  _URL
  _VERSION
  _REQUIRES
  _CONFLICTS
)
  if(NOT TARGET "${TARGET}")
    message(FATAL_ERROR
      "configure_pkg_config_file target '${TARGET}' does not exist"
    )
  endif()

  get_target_property(_TARGET_TYPE "${TARGET}" TYPE)
  set(_SUPPORTED_TARGET_TYPES
    INTERFACE_LIBRARY
    OBJECT_LIBRARY
    STATIC_LIBRARY
    SHARED_LIBRARY
  )
  if(NOT _TARGET_TYPE IN_LIST _SUPPORTED_TARGET_TYPES)
    message(FATAL_ERROR
      "configure_pkg_config_file does not support ${_TARGET_TYPE} target '${TARGET}'"
    )
  endif()

  if("${_NAME}" STREQUAL "")
    get_target_property(_NAME "${TARGET}" NAME)
  endif()

  if("${_DESCRIPTION}" STREQUAL "")
    if(NOT "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}" STREQUAL "")
      set(_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
    elseif(NOT "${CPACK_PACKAGE_DESCRIPTION}" STREQUAL "")
      set(_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
    else()
      set(_DESCRIPTION "${PROJECT_DESCRIPTION}")
    endif()
  endif()
  if("${_VERSION}" STREQUAL "")
    if(NOT "${CPACK_PACKAGE_VERSION}" STREQUAL "")
      set(_VERSION "${CPACK_PACKAGE_VERSION}")
    else()
      set(_VERSION "${PROJECT_VERSION}")
    endif()
  endif()
  if("${_URL}" STREQUAL "")
    if(NOT "${CPACK_PACKAGE_HOMEPAGE_URL}" STREQUAL "")
      set(_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
    else()
      set(_URL "${PROJECT_HOMEPAGE_URL}")
    endif()
  endif()

  if("${_DESCRIPTION}" STREQUAL "")
    message(FATAL_ERROR "configure_pkg_config_file requires DESCRIPTION")
  endif()
  if("${_VERSION}" STREQUAL "")
    message(FATAL_ERROR "configure_pkg_config_file requires VERSION")
  endif()

  if("${_INSTALL_INCLUDE_DIR}" STREQUAL "")
    set(_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
  endif()
  if("${_INSTALL_LIB_DIR}" STREQUAL "")
    set(_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}")
  endif()
  if(IS_ABSOLUTE "${_INSTALL_INCLUDE_DIR}" OR IS_ABSOLUTE "${_INSTALL_LIB_DIR}")
    message(FATAL_ERROR
      "configure_pkg_config_file requires relative install directories"
    )
  endif()

  if(_TARGET_TYPE STREQUAL INTERFACE_LIBRARY)
    set(_IS_INTERFACE TRUE)
  else()
    set(_IS_INTERFACE FALSE)
  endif()
  if(_TARGET_TYPE STREQUAL OBJECT_LIBRARY)
    set(_IS_OBJECT TRUE)
  else()
    set(_IS_OBJECT FALSE)
  endif()

  get_property(_HAS_PUBLIC_INCLUDES
    TARGET "${TARGET}"
    PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    SET
  )

  set(_PKG_CONFIG_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/$<CONFIG>")
  set(_PKG_CONFIG_FILE "${_PKG_CONFIG_OUTPUT_DIR}/${_NAME}.pc")

  set(_PATH_ANCHOR "${CMAKE_CURRENT_BINARY_DIR}/pkgconfig-root")
  file(RELATIVE_PATH _PREFIX_FROM_PC
    "${_PATH_ANCHOR}/${_INSTALL_LIB_DIR}/pkgconfig"
    "${_PATH_ANCHOR}"
  )
  string(REGEX REPLACE "/$" "" _PREFIX_FROM_PC "${_PREFIX_FROM_PC}")

  set(_CONTENT "prefix=\${pcfiledir}/${_PREFIX_FROM_PC}\n")
  if(NOT _IS_INTERFACE)
    string(APPEND _CONTENT "libdir=\${prefix}/${_INSTALL_LIB_DIR}\n")
  endif()
  if(_HAS_PUBLIC_INCLUDES)
    string(APPEND _CONTENT "includedir=\${prefix}/${_INSTALL_INCLUDE_DIR}\n")
  endif()

  string(APPEND _CONTENT
    "\nName: ${_NAME}\n"
    "Description: ${_DESCRIPTION}\n"
  )
  if(NOT "${_URL}" STREQUAL "")
    string(APPEND _CONTENT "URL: ${_URL}\n")
  endif()
  string(APPEND _CONTENT "Version: ${_VERSION}\n")

  if(NOT "${_REQUIRES}" STREQUAL "")
    list(JOIN _REQUIRES ", " _REQUIRES)
    string(APPEND _CONTENT "Requires: ${_REQUIRES}\n")
  endif()
  if(NOT "${_CONFLICTS}" STREQUAL "")
    list(JOIN _CONFLICTS ", " _CONFLICTS)
    string(APPEND _CONTENT "Conflicts: ${_CONFLICTS}\n")
  endif()

  if(_IS_OBJECT)
    string(MD5 _TARGET_ID "${CMAKE_CURRENT_BINARY_DIR};${TARGET};${_NAME}")
    set(_OBJECTS_PLACEHOLDER "__GEN_PKG_CONFIG_OBJECTS_${_TARGET_ID}__")
    set(_TARGET_OBJECTS_FILE "${_PKG_CONFIG_OUTPUT_DIR}/${_NAME}.objects")
    set(_PKG_CONFIG_FILE_UNFINISHED "${_PKG_CONFIG_FILE}.unfinished")
    file(GENERATE
      OUTPUT "${_TARGET_OBJECTS_FILE}"
      CONTENT "$<TARGET_OBJECTS:${TARGET}>"
      TARGET "${TARGET}"
    )
    string(APPEND _CONTENT "Libs: ${_OBJECTS_PLACEHOLDER}\n")
  elseif(NOT _IS_INTERFACE)
    string(APPEND _CONTENT
      "Libs: \${libdir}/$<TARGET_LINKER_FILE_NAME:${TARGET}>\n"
    )
  endif()

  if(_HAS_PUBLIC_INCLUDES)
    string(APPEND _CONTENT "Cflags: -I\${includedir}\n")
  endif()

  set(_INSTALL_COMPONENT_ARGS)
  if(NOT "${_COMPONENT}" STREQUAL "")
    list(APPEND _INSTALL_COMPONENT_ARGS COMPONENT "${_COMPONENT}")
  endif()

  if(_IS_OBJECT)
    file(GENERATE
      OUTPUT "${_PKG_CONFIG_FILE_UNFINISHED}"
      CONTENT "${_CONTENT}"
      TARGET "${TARGET}"
    )

    set(_OBJECTS_HELPER
      "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/buildTimeScripts/getObjectFilesBaseNames.cmake"
    )
    add_custom_command(
      OUTPUT "${_PKG_CONFIG_FILE}"
      COMMAND "${CMAKE_COMMAND}"
        "-DobjectsFile=${_TARGET_OBJECTS_FILE}"
        "-DpkgConfigFileUnfinished=${_PKG_CONFIG_FILE_UNFINISHED}"
        "-DpkgConfigFileFinal=${_PKG_CONFIG_FILE}"
        "-DobjectsPlaceholder=${_OBJECTS_PLACEHOLDER}"
        -P "${_OBJECTS_HELPER}"
      DEPENDS
        "${TARGET}"
        "${_TARGET_OBJECTS_FILE}"
        "${_PKG_CONFIG_FILE_UNFINISHED}"
        "${_OBJECTS_HELPER}"
      COMMENT "Finalize ${_NAME}.pc"
      VERBATIM
    )
    add_custom_target("gen_pkg_config_${_TARGET_ID}" ALL
      DEPENDS "${_PKG_CONFIG_FILE}"
    )

    install(FILES "$<TARGET_OBJECTS:${TARGET}>"
      DESTINATION "${_INSTALL_LIB_DIR}"
      ${_INSTALL_COMPONENT_ARGS}
    )
  else()
    file(GENERATE
      OUTPUT "${_PKG_CONFIG_FILE}"
      CONTENT "${_CONTENT}"
      TARGET "${TARGET}"
    )
  endif()

  install(FILES "${_PKG_CONFIG_FILE}"
    DESTINATION "${_INSTALL_LIB_DIR}/pkgconfig"
    RENAME "${_NAME}.pc"
    ${_INSTALL_COMPONENT_ARGS}
  )
endfunction()
