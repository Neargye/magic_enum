include(CheckCXXCompilerFlag)

if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION AND NOT TARGET magic_enum::magic_enum)
  find_package(magic_enum CONFIG REQUIRED)
endif()

if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(magic_enum REQUIRED IMPORTED_TARGET magic_enum)
endif()

set(MAGIC_ENUM_TEST_STANDARDS 20 23 26)

function(magic_enum_get_standard_flag standard output)
  if(MSVC AND standard GREATER_EQUAL 26)
    set(flag c++latest)
  else()
    set(flag c++${standard})
  endif()
  set(${output} ${flag} PARENT_SCOPE)
endfunction()

if(MSVC)
  set(MAGIC_ENUM_TEST_COMPILE_OPTIONS /W4 /WX)
  check_cxx_compiler_flag(/permissive- MAGIC_ENUM_HAS_PERMISSIVE_FLAG)
  if(MAGIC_ENUM_HAS_PERMISSIVE_FLAG)
    list(APPEND MAGIC_ENUM_TEST_COMPILE_OPTIONS /permissive-)
  endif()
  set(MAGIC_ENUM_TEST_FLAG_PREFIX /std:)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(MAGIC_ENUM_TEST_COMPILE_OPTIONS -Wall -Wextra -Wshadow -pedantic-errors -Werror)
  set(MAGIC_ENUM_TEST_FLAG_PREFIX -std=)

  # Clang 22+ treats __COUNTER__ as C2y extension; but doctest uses it
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "22.0")
    list(APPEND MAGIC_ENUM_TEST_COMPILE_OPTIONS -Wno-c2y-extensions)
  endif()
endif()

foreach(MAGIC_ENUM_TEST_STANDARD IN LISTS MAGIC_ENUM_TEST_STANDARDS)
  magic_enum_get_standard_flag(${MAGIC_ENUM_TEST_STANDARD} MAGIC_ENUM_TEST_STANDARD_FLAG)
  check_cxx_compiler_flag(
    "${MAGIC_ENUM_TEST_FLAG_PREFIX}${MAGIC_ENUM_TEST_STANDARD_FLAG}"
    MAGIC_ENUM_HAS_CPP${MAGIC_ENUM_TEST_STANDARD}_FLAG
  )
endforeach()

# Clang 17 with libstdc++ 14 has a known bug in C++23 mode (tuple/get incompatibility).
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND
   CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "17.0" AND
   CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18.0")
  set(MAGIC_ENUM_HAS_CPP23_FLAG FALSE)
  message(STATUS "Skipping C++23 tests for Clang 17 due to libstdc++ 14 incompatibility")
endif()

if(MAGIC_ENUM_USE_MODULES AND NOT MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION)
  magic_enum_make_module_library(magic_enum_module_test-cpp${MAGIC_ENUM_MODULE_CXX_STANDARD}
    "MAGIC_ENUM_CONFIG_FILE=\"${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_test_config.hpp\""
  )
  magic_enum_make_module_library(magic_enum_module_test_aliases-cpp${MAGIC_ENUM_MODULE_CXX_STANDARD}
    "MAGIC_ENUM_CONFIG_FILE=\"${CMAKE_CURRENT_SOURCE_DIR}/aliases.hpp\""
  )
  magic_enum_make_module_library(magic_enum_module_test_wchar_t-cpp${MAGIC_ENUM_MODULE_CXX_STANDARD}
    "MAGIC_ENUM_CONFIG_FILE=\"${CMAKE_CURRENT_SOURCE_DIR}/magic_enum_wchar_t_test_config.hpp\""
  )
endif()

function(magic_enum_make_test src target standard_flag)
  add_executable(${target} ${src})
  target_compile_options(${target} PRIVATE ${MAGIC_ENUM_TEST_COMPILE_OPTIONS})
  target_compile_definitions(${target} PRIVATE DOCTEST_CONFIG_USE_STD_HEADERS)
  target_include_directories(${target} SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
  if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG)
    target_link_libraries(${target} PRIVATE PkgConfig::magic_enum)
  else()
    target_link_libraries(${target} PRIVATE magic_enum::magic_enum)
  endif()
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
  if(MSVC)
    target_compile_options(${target} PRIVATE /std:${standard_flag})
  else()
    target_compile_options(${target} PRIVATE -std=${standard_flag})
  endif()
  add_test(NAME ${target} COMMAND ${target})
endfunction()

function(magic_enum_make_module_test src target module_target)
  add_executable(${target} ${src})
  target_compile_options(${target} PRIVATE ${MAGIC_ENUM_TEST_COMPILE_OPTIONS})
  if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION)
    target_link_libraries(${target} PRIVATE magic_enum::magic_enum_module)
  else()
    target_link_libraries(${target} PRIVATE ${module_target})
  endif()
  set_target_properties(${target} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  add_test(NAME ${target} COMMAND ${target})
endfunction()

set(MAGIC_ENUM_TEST_SOURCES test test_flags test_aliases test_containers test_wchar_t)

function(magic_enum_add_tests suffix standard_flag)
  foreach(test_name IN LISTS MAGIC_ENUM_TEST_SOURCES)
    magic_enum_make_test(${test_name}.cpp ${test_name}-${suffix} ${standard_flag})
  endforeach()
  if(MAGIC_ENUM_OPT_ENABLE_NONASCII)
    magic_enum_make_test(test_nonascii.cpp test_nonascii-${suffix} ${standard_flag})
  endif()
endfunction()

function(magic_enum_add_module_tests suffix)
  magic_enum_make_module_test(
    test_module.cpp
    test_module-${suffix}
    magic_enum_module_test-${suffix}
  )

  # Installed packages expose one module with the configuration fixed at build
  # time. Alias and wchar coverage use dedicated local module targets instead.
  if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION)
    return()
  endif()

  foreach(test_variant IN ITEMS _aliases _wchar_t)
    magic_enum_make_module_test(
      test_module${test_variant}.cpp
      test_module${test_variant}-${suffix}
      magic_enum_module_test${test_variant}-${suffix}
    )
  endforeach()
endfunction()

magic_enum_add_tests(cpp17 c++17)

foreach(MAGIC_ENUM_TEST_STANDARD IN LISTS MAGIC_ENUM_TEST_STANDARDS)
  set(MAGIC_ENUM_HAS_STANDARD_FLAG MAGIC_ENUM_HAS_CPP${MAGIC_ENUM_TEST_STANDARD}_FLAG)
  if(NOT ${MAGIC_ENUM_HAS_STANDARD_FLAG})
    continue()
  endif()
  if(MAGIC_ENUM_USE_MODULES AND
     NOT MAGIC_ENUM_MODULE_CXX_STANDARD EQUAL MAGIC_ENUM_TEST_STANDARD)
    continue()
  endif()

  magic_enum_get_standard_flag(${MAGIC_ENUM_TEST_STANDARD} MAGIC_ENUM_TEST_STANDARD_FLAG)
  set(MAGIC_ENUM_TEST_SUFFIX cpp${MAGIC_ENUM_TEST_STANDARD})
  magic_enum_add_tests(${MAGIC_ENUM_TEST_SUFFIX} ${MAGIC_ENUM_TEST_STANDARD_FLAG})
  if(MAGIC_ENUM_USE_MODULES)
    magic_enum_add_module_tests(${MAGIC_ENUM_TEST_SUFFIX})
  endif()
endforeach()

if(MAGIC_ENUM_USE_MODULES AND MAGIC_ENUM_OPT_INSTALL AND
   NOT MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION AND
   NOT CMAKE_SKIP_INSTALL_RULES AND
   CMAKE_GENERATOR STREQUAL "Ninja")
  set(MAGIC_ENUM_STAGED_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/stagedir")
  set(MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/installed-module-consumer")
  set(MAGIC_ENUM_INSTALLED_CONSUMER_PREFIX_PATH "${MAGIC_ENUM_STAGED_INSTALL_DIR}")
  foreach(MAGIC_ENUM_PREFIX IN LISTS CMAKE_PREFIX_PATH)
    string(APPEND MAGIC_ENUM_INSTALLED_CONSUMER_PREFIX_PATH "\\;${MAGIC_ENUM_PREFIX}")
  endforeach()
  set(MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_OPTIONS
      "-DCMAKE_BUILD_TYPE=$<CONFIG>"
      "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
      "-DCMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS}"
      "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
      "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
      "-DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED}"
      "-DCMAKE_PREFIX_PATH=${MAGIC_ENUM_INSTALLED_CONSUMER_PREFIX_PATH}"
      "-DMAGIC_ENUM_EXPECT_IMPORT_STD=${MAGIC_ENUM_MODULE_IMPORT_STD}"
      "-DMAGIC_ENUM_EXPECT_FMT=${MAGIC_ENUM_MODULE_WITH_FMT}"
  )
  if(MSVC)
    find_program(MAGIC_ENUM_RC_COMPILER rc REQUIRED)
    find_program(MAGIC_ENUM_MT mt REQUIRED)
    list(APPEND MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_OPTIONS
         "-DCMAKE_RC_COMPILER=${MAGIC_ENUM_RC_COMPILER}"
         "-DCMAKE_MT=${MAGIC_ENUM_MT}"
    )
  endif()
  if(MAGIC_ENUM_MODULE_IMPORT_STD)
    list(APPEND MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_OPTIONS
         "-DCMAKE_EXPERIMENTAL_CXX_IMPORT_STD=${CMAKE_EXPERIMENTAL_CXX_IMPORT_STD}"
    )
    if(CMAKE_CXX_STDLIB_MODULES_JSON)
      list(APPEND MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_OPTIONS
           "-DCMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON}"
      )
    endif()
  endif()

  add_test(
    NAME install-module-to-stagedir
    COMMAND
      ${CMAKE_COMMAND}
      "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
      "-DSTAGE_DIR=${MAGIC_ENUM_STAGED_INSTALL_DIR}"
      "-DCONSUMER_BUILD_DIR=${MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_DIR}"
      "-DCONFIG=$<CONFIG>"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/stage_package.cmake"
  )
  set_tests_properties(install-module-to-stagedir PROPERTIES FIXTURES_SETUP magic_enum_installed_module)

  add_test(
    NAME installed-module-consumer
    COMMAND
      ${CMAKE_CTEST_COMMAND}
      --build-and-test
      "${CMAKE_CURRENT_SOURCE_DIR}/installed_module"
      "${MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_DIR}"
      --build-generator "${CMAKE_GENERATOR}"
      --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
      --build-options ${MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_OPTIONS}
      --test-command
      ${CMAKE_CTEST_COMMAND} --output-on-failure --no-tests=error -C $<CONFIG>
  )
  set_tests_properties(installed-module-consumer PROPERTIES FIXTURES_REQUIRED magic_enum_installed_module)
endif()
