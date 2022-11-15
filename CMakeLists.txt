cmake_minimum_required(VERSION 3.14)

project(magic_enum VERSION "0.8.1" LANGUAGES CXX)

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

include(CMakePackageConfigHelpers)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT)

if(MAGIC_ENUM_OPT_INSTALL)
    include(GNUInstallDirs)

    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Config)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            DESTINATION lib/cmake/${PROJECT_NAME})

    install(EXPORT ${PROJECT_NAME}Config
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION lib/cmake/${PROJECT_NAME})

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
            DESTINATION .)

    export(EXPORT ${PROJECT_NAME}Config
            NAMESPACE ${PROJECT_NAME}::)

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/package.xml
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
endif()
