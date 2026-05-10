cmake_minimum_required(VERSION 3.14...4.20)

project(
  magic_enum
  VERSION "0.9.8"
  HOMEPAGE_URL "https://github.com/Neargye/magic_enum"
  DESCRIPTION
    "A library that provides static reflection for enums, work with any enum type without any macro or boilerplate code."
  LANGUAGES CXX
)
set(CPACK_PACKAGE_VENDOR "Daniil Goncharov")

include(GNUInstallDirs)

set(ADDITIONAL_MODULES_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${ADDITIONAL_MODULES_DIR}")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(IS_TOPLEVEL_PROJECT TRUE)
else()
  set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(MAGIC_ENUM_OPT_BUILD_EXAMPLES "Build magic_enum examples" ${IS_TOPLEVEL_PROJECT})
option(MAGIC_ENUM_OPT_BUILD_TESTS "Build and perform magic_enum tests" ${IS_TOPLEVEL_PROJECT})
option(MAGIC_ENUM_OPT_INSTALL "Generate and install magic_enum target" ${IS_TOPLEVEL_PROJECT})
option(MAGIC_ENUM_OPT_INSTALL_PACKAGE_XML "Include package.xml when installing"
       ${MAGIC_ENUM_OPT_INSTALL}
)
option(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION "When configuring tests, try use
a local magic_enum." NO
)
option(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG "When configuring tests, try use
a local magic_enum via pkgconfig" NO
)

if(${MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION} OR ${MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG})
  add_subdirectory(test)
  return()
endif()

set(INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/magic_enum")
set(MODULE "${CMAKE_CURRENT_SOURCE_DIR}/module")
set(EXPORT_NAMESPACE "${PROJECT_NAME}::")

if(MAGIC_ENUM_USE_MODULES)
  set(CMAKE_CXX_EXTENSIONS OFF)
  if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
  endif()
endif()

function(magic_enum_make_library name)

if(MAGIC_ENUM_USE_MODULES)
  add_library(${name})
  target_sources(${name}
    PUBLIC
      FILE_SET CXX_MODULES
      BASE_DIRS "${MODULE}"
      FILES "${MODULE}/magic_enum.cppm"
    PUBLIC
      FILE_SET HEADERS
      BASE_DIRS "${INCLUDES}"
      FILES
        "${INCLUDES}/magic_enum.hpp"
        "${INCLUDES}/magic_enum_all.hpp"
        "${INCLUDES}/magic_enum_containers.hpp"
        "${INCLUDES}/magic_enum_flags.hpp"
        "${INCLUDES}/magic_enum_format.hpp"
        "${INCLUDES}/magic_enum_fuse.hpp"
        "${INCLUDES}/magic_enum_iostream.hpp"
        "${INCLUDES}/magic_enum_switch.hpp"
        "${INCLUDES}/magic_enum_utility.hpp"
        "${INCLUDES}/detail/config.hpp"
  )
  target_compile_features(${name} PUBLIC cxx_std_20)
  target_compile_definitions(${name}
    PUBLIC MAGIC_ENUM_USE_MODULES
    PRIVATE ${ARGN}
  )
  if((CMAKE_CXX_STANDARD IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD) AND CMAKE_CXX_MODULE_STD)
    message("magic_enum: using import std")
    target_compile_features(${name} PRIVATE cxx_std_23)
    target_compile_definitions(${name} PRIVATE MAGIC_ENUM_USE_IMPORT_STD)
  endif()

  set(__scope PUBLIC)
else()
  add_library(${name} INTERFACE)
  set(__scope INTERFACE)
endif()

add_library(${EXPORT_NAMESPACE}${name} ALIAS ${name})
target_include_directories(
  ${name} ${__scope} $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

endfunction()

magic_enum_make_library(${PROJECT_NAME})

if(MAGIC_ENUM_OPT_BUILD_EXAMPLES)
  add_subdirectory(example)
endif()

if(MAGIC_ENUM_OPT_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

if(MAGIC_ENUM_OPT_INSTALL)
  list(APPEND CMAKE_MODULE_PATH "${ADDITIONAL_MODULES_DIR}/GenPkgConfig")
  include(GenPkgConfig)
  include(CPackComponent)
  include(CMakePackageConfigHelpers)

  if(MAGIC_ENUM_USE_MODULES)
    install(
      TARGETS "${PROJECT_NAME}"
      EXPORT ${PROJECT_NAME}
      INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
      FILE_SET HEADERS
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
      FILE_SET CXX_MODULES
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}"
      CXX_MODULES_BMI
        DESTINATION ""
    )
  else()
    install(
      TARGETS "${PROJECT_NAME}"
      EXPORT ${PROJECT_NAME}
      INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
      # COMPONENT "${SDK_COMPONENT_NAME}" # component is not allowed for
      # includes!
      # Headers are installed separately! Includes only marks the headers for
      # export
    )

    if(NOT DEFINED PROJECT_INCLUDE_HEADER_PATTERN)
      set(PROJECT_INCLUDE_HEADER_PATTERN "*")
    endif()

    install(
      DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
      FILES_MATCHING
      PATTERN "${PROJECT_INCLUDE_HEADER_PATTERN}"
    )
  endif()

  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
  set(CPACK_DEBIAN_PACKAGE_NAME "libmagicenum-dev")
  set(CPACK_RPM_PACKAGE_NAME "libmagicenum-devel")
  set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
  set(CPACK_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_MAINTAINER}")
  set(CPACK_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
  set(CPACK_DEB_COMPONENT_INSTALL ON)
  set(CPACK_RPM_COMPONENT_INSTALL ON)
  set(CPACK_NSIS_COMPONENT_INSTALL ON)
  set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")

  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

  set(CMAKE_CONFIG_FILE_BASENAME "${PROJECT_NAME}Config.cmake")
  set(CMAKE_EXPORT_FILE_BASENAME "${PROJECT_NAME}Export.cmake")
  set(CMAKE_CONFIG_VERSION_FILE_BASENAME "${PROJECT_NAME}ConfigVersion.cmake")
  set(CMAKE_CONFIG_VERSION_FILE_NAME
      "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CONFIG_VERSION_FILE_BASENAME}"
  )

  export(
    TARGETS "${PROJECT_NAME}"
    NAMESPACE "${EXPORT_NAMESPACE}"
    FILE "${CMAKE_EXPORT_FILE_BASENAME}"
    EXPORT_LINK_INTERFACE_LIBRARIES
  )

  install(
    EXPORT "${PROJECT_NAME}"
    FILE "${CMAKE_CONFIG_FILE_BASENAME}"
    NAMESPACE "${EXPORT_NAMESPACE}"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}"
  )

  write_basic_package_version_file(
    "${CMAKE_CONFIG_VERSION_FILE_NAME}"
    # VERSION "100500.100500.100500"  # any version of same bitness suits. CMake cannot compare to
    # infinity, so use a large number we expect to be greater than any future version
    VERSION ${_VERSION}
    COMPATIBILITY AnyNewerVersion ARCH_INDEPENDENT
  )
  install(FILES "${CMAKE_CONFIG_VERSION_FILE_NAME}"
          DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}"
  )

  configure_pkg_config_file(
    "${PROJECT_NAME}"
    NAME
    "${PROJECT_NAME}"
    VERSION
    "${PROJECT_VERSION}"
    DESCRIPTION
    "${CPACK_PACKAGE_DESCRIPTION}"
    URL
    "${CPACK_PACKAGE_HOMEPAGE_URL}"
    INSTALL_LIB_DIR
    "${CMAKE_INSTALL_DATAROOTDIR}"
    INSTALL_INCLUDE_DIR
    "${CMAKE_INSTALL_INCLUDEDIR}"
  )

  if(MAGIC_ENUM_OPT_INSTALL_PACKAGE_XML)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/package.xml
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    )
  endif()

  include(CPack)
endif()
