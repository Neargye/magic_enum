include(CheckCXXCompilerFlag)

set(OPTIONS "")

if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(OPTIONS -Wall -Wextra -pedantic-errors)
    set(OPTIONS ${OPTIONS} -std=c++17)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(OPTIONS /W4)
    check_cxx_compiler_flag(/permissive HAS_PERMISSIVE_FLAG)
    if(HAS_PERMISSIVE_FLAG)
        set(OPTIONS ${OPTIONS} /permissive-)
    endif()
    set(OPTIONS ${OPTIONS} /std:c++17)
endif()

add_executable(example
                    example.cpp
                    ${CMAKE_SOURCE_DIR}/include/${CMAKE_PROJECT_NAME}.hpp)
target_compile_options(example PRIVATE ${OPTIONS})
target_link_libraries(example PRIVATE ${CMAKE_PROJECT_NAME})
