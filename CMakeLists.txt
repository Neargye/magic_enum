cmake_minimum_required(VERSION 3.6)

project(magic_enum VERSION "0.4.1" LANGUAGES CXX)

option(MAGIC_ENUM_OPT_BUILD_EXAMPLES "Build magic_enum examples" ON)
option(MAGIC_ENUM_OPT_BUILD_TESTS "Build and perform magic_enum tests" ON)

if(MAGIC_ENUM_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(MAGIC_ENUM_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
