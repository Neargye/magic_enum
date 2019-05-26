include(CheckCXXCompilerFlag)

if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    check_cxx_compiler_flag(-std=c++17 HAS_CPP17_FLAG)
    if(!HAS_CPP17_FLAG)
        MESSAGE(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
    endif()

    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(OPTIONS -Wall -Wextra -pedantic-errors -Werror -std=c++17)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    check_cxx_compiler_flag(/std:c++17 HAS_CPP17_FLAG)
    if(!HAS_CPP17_FLAG)
        MESSAGE(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
    endif()

    set(OPTIONS /W4 /WX /std:c++17)
    if(HAS_PERMISSIVE_FLAG)
        set(OPTIONS ${OPTIONS} /permissive-)
    endif()
endif()

add_executable(example
                    example.cpp
                    ${CMAKE_SOURCE_DIR}/include/${CMAKE_PROJECT_NAME}.hpp)
target_compile_options(example PRIVATE ${OPTIONS})
target_link_libraries(example PRIVATE ${CMAKE_PROJECT_NAME})
