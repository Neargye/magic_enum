cmake_minimum_required(VERSION 3.14)

project(magic_enum
    VERSION "0.8.2"
    HOMEPAGE_URL "https://github.com/Neargye/magic_enum"
    DESCRIPTION "A library that provides static reflection for enums, work with any enum type without any macro or boilerplate code."
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

option(MAGIC_ENUM_OPT_ENABLE_NONASCII "Enable support for non-ASCII enumeration identifier" OFF)
option(MAGIC_ENUM_OPT_BUILD_EXAMPLES "Build magic_enum examples" ${IS_TOPLEVEL_PROJECT})
option(MAGIC_ENUM_OPT_BUILD_TESTS "Build and perform magic_enum tests" ${IS_TOPLEVEL_PROJECT})
option(MAGIC_ENUM_OPT_INSTALL "Generate and install magic_enum target" ${IS_TOPLEVEL_PROJECT})

if(MAGIC_ENUM_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(MAGIC_ENUM_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

set(INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(EXPORT_NAMESPACE "${PROJECT_NAME}::")

add_library(${PROJECT_NAME} INTERFACE)
add_library(${EXPORT_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${INCLUDES}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


if(MAGIC_ENUM_OPT_INSTALL)
    list(APPEND CMAKE_MODULE_PATH "${ADDITIONAL_MODULES_DIR}/GenPkgConfig")
    include(GenPkgConfig)
    include(CPackComponent)
    include(CMakePackageConfigHelpers)
    

    install(TARGETS "${PROJECT_NAME}"
        EXPORT ${PROJECT_NAME}
        INCLUDES
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            # COMPONENT "${SDK_COMPONENT_NAME}" # component is not allowed for includes! Headers are installed separately! Includes only marks the headers for export
    )

    file(GLOB_RECURSE HEADERS "${INCLUDES}/*.h" "${INCLUDES}/*.hxx" "${INCLUDES}/*.hpp")
    string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
    foreach(headerFile ${HEADERS})
        get_filename_component(headerFileParentDir "${headerFile}" DIRECTORY)
        file(RELATIVE_PATH headerFileRelParentDir "${INCLUDES}" "${headerFileParentDir}")

        install(FILES "${headerFile}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${headerFileRelParentDir}"
        )
    endforeach()

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
    set(CMAKE_CONFIG_VERSION_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CONFIG_VERSION_FILE_BASENAME}")

    export(TARGETS "${PROJECT_NAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        FILE "${CMAKE_EXPORT_FILE_BASENAME}"
        EXPORT_LINK_INTERFACE_LIBRARIES
    )

    install(EXPORT "${PROJECT_NAME}"
        FILE "${CMAKE_CONFIG_FILE_BASENAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}"
    )

    write_basic_package_version_file(
        "${CMAKE_CONFIG_VERSION_FILE_NAME}"
        #VERSION "100500.100500.100500"  # any version of same bitness suits. CMake cannot compare to infinity, so use a large number we expect to be greater than any future version
        VERSION ${_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT
    )
    install(FILES "${CMAKE_CONFIG_VERSION_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${CMAKE_EXPORT_NAME}"
    )

    configure_pkg_config_file("${PROJECT_NAME}"
        NAME "${PROJECT_NAME}"
        VERSION "${PROJECT_VERSION}"
        DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}"
        URL "${CPACK_PACKAGE_HOMEPAGE_URL}"
        INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR_ARCHIND}"
        INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/package.xml
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
    include(CPack)
endif()
