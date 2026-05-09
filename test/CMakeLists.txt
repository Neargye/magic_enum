include(CheckCXXCompilerFlag)

if(${MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION})
  find_package(magic_enum REQUIRED magic_enum)
endif()

if(${MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG})
  find_package(PkgConfig)
  pkg_check_modules(magic_enum magic_enum)
  if(NOT magic_enum_FOUND)
    message(
      WARNING
        "magic_enum via pkgconfig is not found. \
Next code will try check possible places for some platforms, \
but there's no guarantee. \
If you know where the magic_enum pkgconfig files (.pc) are, \
then specify yourself variable \$\{CMAKE_PREFIX_PATH\} \
with folder like /a/path/to/magic_enum (for POSIX-like pathes), \
where in the folder exists share/pkgconfig/magic_enum.pc ."
    )
    if(UNIX AND EXISTS "/usr/local/share/pkgconfig/magic_enum.pc")
      message(DEBUG "\$\{CMAKE_PREFIX_PATH\} : ${CMAKE_PREFIX_PATH} ")
      set(CMAKE_PREFIX_PATH "/usr/local")
      message(DEBUG "\$\{CMAKE_PREFIX_PATH\} : ${CMAKE_PREFIX_PATH} ")
      pkg_check_modules(magic_enum magic_enum)
    endif()
    # code place for future workarounds for other platforms...
    if(NOT magic_enum_FOUND)
      message(FATAL_ERROR "Could not find magic_enum's config. Read a warning above.")
    endif()
  endif()
  message(DEBUG "magic_enum_FOUND : ${magic_enum_FOUND}")
  message(DEBUG "magic_enum_LIBRARIES: ${magic_enum_LIBRARIES}")
  message(DEBUG "magic_enum_LINK_LIBRARIES: ${magic_enum_LINK_LIBRARIES}")
  message(DEBUG "magic_enum_LIBRARY_DIRS: ${magic_enum_LIBRARY_DIRS}")
  message(DEBUG "magic_enum_INCLUDE_DIRS: ${magic_enum_INCLUDE_DIRS}")
  message(DEBUG "magic_enum_INCLUDE_DIR: ${magic_enum_INCLUDE_DIR}")
  message(DEBUG "magic_enum_LDFLAGS: ${magic_enum_LDFLAGS}")
  message(DEBUG "magic_enum_LDFLAGS_OTHER: ${magic_enum_LDFLAGS_OTHER}")
  message(DEBUG "magic_enum_CFLAGS: ${magic_enum_CFLAGS}")
  message(DEBUG "magic_enum_CFLAGS_OTHER: ${magic_enum_CFLAGS_OTHER}")
  message(DEBUG "magic_enum_INCLUDEDIR: ${magic_enum_INCLUDEDIR}")
  message(DEBUG "magic_enum_LIBDIR: ${magic_enum_LIBDIR}")
  message(DEBUG "magic_enum_PREFIX: ${magic_enum_PREFIX}")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(OPTIONS /W4 /WX)
  check_cxx_compiler_flag(/permissive HAS_PERMISSIVE_FLAG)
  if(HAS_PERMISSIVE_FLAG)
    set(OPTIONS ${OPTIONS} /permissive-)
  endif()

  check_cxx_compiler_flag(/std:c++20 HAS_CPP20_FLAG)
  check_cxx_compiler_flag(/std:c++23 HAS_CPP23_FLAG)
  check_cxx_compiler_flag(/std:c++latest HAS_CPPLATEST_FLAG)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(OPTIONS -Wall -Wextra -Wshadow -pedantic-errors -Werror)

  # Clang 22+ treats __COUNTER__ as C2y extension, but Catch2 v2 uses it
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "22.0")
    set(OPTIONS ${OPTIONS} -Wno-c2y-extensions)
  endif()

  check_cxx_compiler_flag(-std=c++20 HAS_CPP20_FLAG)
  check_cxx_compiler_flag(-std=c++23 HAS_CPP23_FLAG)

  # Clang 17 with libstdc++ 14 has a known bug in C++23 mode (tuple/get incompatibility)
  # Skip C++23 tests for Clang 17.x to avoid compilation errors
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "17.0" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18.0")
    set(HAS_CPP23_FLAG FALSE)
    message(STATUS "Skipping C++23 tests for Clang 17 due to libstdc++ 14 incompatibility")
  endif()
endif()

if(MAGIC_ENUM_USE_MODULES AND NOT MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION)

magic_enum_make_library(magic_enum_module_test-cpp${CMAKE_CXX_STANDARD}
  MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
  MAGIC_ENUM_RANGE_MIN=-120
  MAGIC_ENUM_RANGE_MAX=120
)
magic_enum_make_library(magic_enum_module_test_aliases-cpp${CMAKE_CXX_STANDARD}
  "MAGIC_ENUM_CONFIG_FILE=\"../../test/aliases.hpp\""
)
magic_enum_make_library(magic_enum_module_test_wchar_t-cpp${CMAKE_CXX_STANDARD}
  "MAGIC_ENUM_USING_ALIAS_STRING_VIEW=using string_view = std::wstring_view"
  "MAGIC_ENUM_USING_ALIAS_STRING=using string = std::wstring"
)

endif()

function(make_test src target std)
  add_executable(${target} ${src})
  target_compile_options(${target} PRIVATE ${OPTIONS})
  target_include_directories(${target} PRIVATE 3rdparty/Catch2/include)
  if(${MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG})
    target_include_directories(${target} PRIVATE ${magic_enum_INCLUDE_DIRS})
  elseif(TARGET magic_enum_module_${target})
    target_link_libraries(${target} PRIVATE magic_enum_module_${target})
  else()
    target_link_libraries(${target} PRIVATE magic_enum::magic_enum)
  endif()
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
  if(std)
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
      target_compile_options(${target} PRIVATE /std:${std})
    else()
      target_compile_options(${target} PRIVATE -std=${std})
    endif()
  endif()
  add_test(NAME ${target} COMMAND ${target})
endfunction()

if(NOT MAGIC_ENUM_USE_MODULES)

make_test(test.cpp test-cpp17 c++17)
make_test(test_flags.cpp test_flags-cpp17 c++17)
make_test(test_aliases.cpp test_aliases-cpp17 c++17)
make_test(test_containers.cpp test_containers-cpp17 c++17)
make_test(test_wchar_t.cpp test_wchar_t-cpp17 c++17)

if(MAGIC_ENUM_OPT_ENABLE_NONASCII)
  make_test(test_nonascii.cpp test_nonascii-cpp17 c++17)
endif()

endif()

if(HAS_CPP20_FLAG AND (NOT MAGIC_ENUM_USE_MODULES OR (CMAKE_CXX_STANDARD EQUAL 20)))
  make_test(test.cpp test-cpp20 c++20)
  make_test(test_flags.cpp test_flags-cpp20 c++20)
  make_test(test_aliases.cpp test_aliases-cpp20 c++20)
  make_test(test_containers.cpp test_containers-cpp20 c++20)
  make_test(test_wchar_t.cpp test_wchar_t-cpp20 c++20)
  if(MAGIC_ENUM_OPT_ENABLE_NONASCII)
    make_test(test_nonascii.cpp test_nonascii-cpp20 c++20)
  endif()
endif()

if(HAS_CPP23_FLAG AND (NOT MAGIC_ENUM_USE_MODULES OR (CMAKE_CXX_STANDARD EQUAL 23)))
  make_test(test.cpp test-cpp23 c++23)
  make_test(test_flags.cpp test_flags-cpp23 c++23)
  make_test(test_aliases.cpp test_aliases-cpp23 c++23)
  make_test(test_containers.cpp test_containers-cpp23 c++23)
  make_test(test_wchar_t.cpp test_wchar_t-cpp23 c++23)
  if(MAGIC_ENUM_OPT_ENABLE_NONASCII)
    make_test(test_nonascii.cpp test_nonascii-cpp23 c++23)
  endif()
endif()

if(HAS_CPPLATEST_FLAG)
  make_test(test.cpp test-cpplatest c++latest)
  make_test(test_flags.cpp test_flags-cpplatest c++latest)
  make_test(test_aliases.cpp test_aliases-cpplatest c++latest)
  make_test(test_containers.cpp test_containers-cpplatest c++latest)
  make_test(test_wchar_t.cpp test_wchar_t-cpplatest c++latest)
  if(MAGIC_ENUM_OPT_ENABLE_NONASCII)
    make_test(test_nonascii.cpp test_nonascii-cpplatest c++latest)
  endif()
endif()
