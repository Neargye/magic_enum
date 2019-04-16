cmake_minimum_required(VERSION 3.6)

project(magic_enum VERSION "0.4.2" LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else ()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif ()

option(MAGIC_ENUM_OPT_BUILD_EXAMPLES "Build magic_enum examples" ${IS_TOPLEVEL_PROJECT})
option(MAGIC_ENUM_OPT_BUILD_TESTS "Build and perform magic_enum tests" ${IS_TOPLEVEL_PROJECT})

if(MAGIC_ENUM_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(MAGIC_ENUM_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
