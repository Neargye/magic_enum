include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

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

set(MAGIC_ENUM_TEST_STD_REFLECTION_COMPILE_OPTIONS)
set(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION FALSE)
if(MAGIC_ENUM_HAS_CPP26_FLAG)
  magic_enum_get_standard_flag(26 MAGIC_ENUM_CPP26_STANDARD_FLAG)
  set(MAGIC_ENUM_STD_REFLECTION_PROBE [[
    #include <meta>
    enum class reflected_enum { value };
    static constexpr auto enumerators = std::define_static_array(std::meta::enumerators_of(^^reflected_enum));
    static constexpr auto name = std::define_static_string("value");
    static_assert(enumerators.size() == 1);
    static_assert(name[5] == char{});
    consteval int enumerator_count() {
      int count = 0;
      template for (constexpr auto enumerator : enumerators) {
        static_assert(std::meta::identifier_of(enumerator).size() == 5);
        ++count;
      }
      return count;
    }
    static_assert(enumerator_count() == 1);
    int main() {}
  ]])
  set(MAGIC_ENUM_SAVED_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${MAGIC_ENUM_TEST_FLAG_PREFIX}${MAGIC_ENUM_CPP26_STANDARD_FLAG}")
  check_cxx_source_compiles(
    "${MAGIC_ENUM_STD_REFLECTION_PROBE}"
    MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_WITH_EXPANSION_BY_DEFAULT
  )

  if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_WITH_EXPANSION_BY_DEFAULT)
    set(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION TRUE)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Prefer reflection provided by the active C++26 mode. Use GNU's temporary opt-in only when the default probe fails.
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -freflection")
    check_cxx_source_compiles(
      "${MAGIC_ENUM_STD_REFLECTION_PROBE}"
      MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_WITH_EXPANSION_GNU_OPTION
    )
    if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_WITH_EXPANSION_GNU_OPTION)
      set(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION TRUE)
      list(APPEND MAGIC_ENUM_TEST_STD_REFLECTION_COMPILE_OPTIONS -freflection)
    endif()
  endif()

  set(CMAKE_REQUIRED_FLAGS "${MAGIC_ENUM_SAVED_REQUIRED_FLAGS}")
  unset(MAGIC_ENUM_SAVED_REQUIRED_FLAGS)
  unset(MAGIC_ENUM_STD_REFLECTION_PROBE)
endif()

function(magic_enum_add_std_reflection_options target)
  if(MAGIC_ENUM_TEST_STD_REFLECTION_COMPILE_OPTIONS)
    target_compile_options(${target} PRIVATE ${MAGIC_ENUM_TEST_STD_REFLECTION_COMPILE_OPTIONS})
  endif()
endfunction()

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

if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION AND MAGIC_ENUM_USE_MODULES AND
   MAGIC_ENUM_MODULE_CXX_STANDARD GREATER_EQUAL 26)
  foreach(module_target IN ITEMS
      magic_enum_module
      magic_enum_module_test-cpp${MAGIC_ENUM_MODULE_CXX_STANDARD}
      magic_enum_module_test_aliases-cpp${MAGIC_ENUM_MODULE_CXX_STANDARD}
      magic_enum_module_test_wchar_t-cpp${MAGIC_ENUM_MODULE_CXX_STANDARD})
    if(TARGET ${module_target})
      magic_enum_add_std_reflection_options(${module_target})
    endif()
  endforeach()
  if(TARGET example_module_usage)
    magic_enum_add_std_reflection_options(example_module_usage)
  endif()
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
  if(standard_flag MATCHES "^c\\+\\+(17|20|23)$")
    set_target_properties(
      ${target}
      PROPERTIES
        CXX_STANDARD "${CMAKE_MATCH_1}"
        CXX_STANDARD_REQUIRED ON
    )
  elseif(MSVC)
    target_compile_options(${target} PRIVATE /std:${standard_flag})
  else()
    target_compile_options(${target} PRIVATE -std=${standard_flag})
  endif()
  add_test(NAME ${target} COMMAND ${target})
endfunction()

function(magic_enum_make_compile_fail_test case_name case_id expected_text)
  set(target test_reflection-compile-fail-${case_name}-cpp26)
  add_executable(${target} EXCLUDE_FROM_ALL test_reflection_compile_fail.cpp)
  target_compile_options(${target} PRIVATE
    ${MAGIC_ENUM_TEST_COMPILE_OPTIONS}
    ${MAGIC_ENUM_TEST_FLAG_PREFIX}${MAGIC_ENUM_CPP26_STANDARD_FLAG}
    ${MAGIC_ENUM_TEST_STD_REFLECTION_COMPILE_OPTIONS}
  )
  target_compile_definitions(${target} PRIVATE
    MAGIC_ENUM_REFLECTION_COMPILE_FAIL_CASE=${case_id}
    ${ARGN}
  )
  if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG)
    target_link_libraries(${target} PRIVATE PkgConfig::magic_enum)
  else()
    target_link_libraries(${target} PRIVATE magic_enum::magic_enum)
  endif()
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)

  add_test(
    NAME ${target}
    COMMAND ${CMAKE_COMMAND}
      "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
      "-DTARGET_NAME=${target}"
      "-DCONFIG=$<CONFIG>"
      "-DEXPECTED_TEXT=${expected_text}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/check_compile_fail.cmake"
  )
  set_tests_properties(${target} PROPERTIES RUN_SERIAL TRUE)
endfunction()

function(magic_enum_make_configuration_compile_fail_test case_name case_id expected_text)
  set(target test_configuration-compile-fail-${case_name}-cpp17)
  add_executable(${target} EXCLUDE_FROM_ALL test_configuration_compile_fail.cpp)
  target_compile_options(${target} PRIVATE ${MAGIC_ENUM_TEST_COMPILE_OPTIONS})
  target_compile_definitions(${target} PRIVATE MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE=${case_id})
  if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG)
    target_link_libraries(${target} PRIVATE PkgConfig::magic_enum)
  else()
    target_link_libraries(${target} PRIVATE magic_enum::magic_enum)
  endif()
  set_target_properties(
    ${target}
    PROPERTIES
      CXX_EXTENSIONS OFF
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
  )

  add_test(
    NAME ${target}
    COMMAND ${CMAKE_COMMAND}
      "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
      "-DTARGET_NAME=${target}"
      "-DCONFIG=$<CONFIG>"
      "-DEXPECTED_TEXT=${expected_text}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/check_compile_fail.cmake"
  )
  set_tests_properties(${target} PROPERTIES RUN_SERIAL TRUE)
endfunction()

function(magic_enum_make_module_test src target module_target)
  add_executable(${target} ${src})
  target_compile_options(${target} PRIVATE ${MAGIC_ENUM_TEST_COMPILE_OPTIONS})
  if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION)
    target_link_libraries(${target} PRIVATE magic_enum::magic_enum_module)
  else()
    target_link_libraries(${target} PRIVATE ${module_target})
  endif()
  set_target_properties(
    ${target}
    PROPERTIES
      CXX_EXTENSIONS OFF
      CXX_SCAN_FOR_MODULES ON
  )
  if(MAGIC_ENUM_MODULE_IMPORT_STD)
    target_compile_definitions(${target} PRIVATE MAGIC_ENUM_TEST_IMPORT_STD)
    set_property(TARGET ${target} PROPERTY CXX_MODULE_STD ON)
  endif()
  if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION AND
     MAGIC_ENUM_MODULE_CXX_STANDARD GREATER_EQUAL 26)
    magic_enum_add_std_reflection_options(${target})
    target_compile_definitions(${target} PRIVATE MAGIC_ENUM_TEST_STD_REFLECTION=1)
  endif()
  add_test(NAME ${target} COMMAND ${target})
endfunction()

set(MAGIC_ENUM_TEST_SOURCES test test_flags test_aliases test_containers test_wchar_t)

function(magic_enum_add_tests suffix standard_flag)
  foreach(test_name IN LISTS MAGIC_ENUM_TEST_SOURCES)
    magic_enum_make_test(${test_name}.cpp ${test_name}-${suffix} ${standard_flag})
  endforeach()
endfunction()

function(magic_enum_add_module_tests suffix)
  magic_enum_make_module_test(
    test_module.cpp
    test_module-${suffix}
    magic_enum_module_test-${suffix}
  )
  if(MAGIC_ENUM_MODULE_WITH_FMT)
    target_compile_definitions(test_module-${suffix} PRIVATE MAGIC_ENUM_TEST_FMT)
  endif()

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
magic_enum_make_test(test_range.cpp test_range-cpp17 c++17)
magic_enum_make_test(test.cpp test-hash-cpp17 c++17)
target_compile_definitions(test-hash-cpp17 PRIVATE MAGIC_ENUM_ENABLE_HASH=1)
magic_enum_make_test(test.cpp test-hash-switch-cpp17 c++17)
target_compile_definitions(test-hash-switch-cpp17 PRIVATE MAGIC_ENUM_ENABLE_HASH_SWITCH=1)
magic_enum_make_configuration_compile_fail_test(
  conflicting-assert
  1
  "MAGIC_ENUM_NO_ASSERT and MAGIC_ENUM_ASSERT cannot be used together"
)
magic_enum_make_configuration_compile_fail_test(
  invalid-is-flags
  2
  "magic_enum::customize::enum_range requires is_flags to have bool type"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_definitions(test-cpp17 PRIVATE MAGIC_ENUM_TEST_VS_2017_WORKAROUND=1)
  if(MSVC_VERSION GREATER_EQUAL 1920)
    # Exercise the Visual Studio 2017 fallback on current MSVC too.
    target_compile_definitions(test-cpp17 PRIVATE MAGIC_ENUM_VS_2017_WORKAROUND=1)
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(MAGIC_ENUM_TEST_CLR_OBJECT "${CMAKE_CURRENT_BINARY_DIR}/test_clr.obj")
  add_custom_command(
    OUTPUT "${MAGIC_ENUM_TEST_CLR_OBJECT}"
    COMMAND "${CMAKE_CXX_COMPILER}" /nologo /std:c++17 /clr /EHa /W4 /WX /permissive-
            "/I${PROJECT_SOURCE_DIR}/include" /c "${PROJECT_SOURCE_DIR}/example/example.cpp"
            "/Fo${MAGIC_ENUM_TEST_CLR_OBJECT}"
    DEPENDS "${PROJECT_SOURCE_DIR}/example/example.cpp" "${PROJECT_SOURCE_DIR}/include/magic_enum/magic_enum.hpp"
    VERBATIM
  )
  add_custom_target(test_clr DEPENDS "${MAGIC_ENUM_TEST_CLR_OBJECT}")
endif()

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
  if(MAGIC_ENUM_TEST_STANDARD EQUAL 26 AND MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_WITH_EXPANSION_BY_DEFAULT)
    foreach(test_name IN LISTS MAGIC_ENUM_TEST_SOURCES)
      target_compile_definitions(${test_name}-${MAGIC_ENUM_TEST_SUFFIX} PRIVATE MAGIC_ENUM_TEST_STD_REFLECTION=1)
    endforeach()
  endif()
endforeach()

if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION)
  magic_enum_add_tests(cpp26-reflection ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
  foreach(test_name IN LISTS MAGIC_ENUM_TEST_SOURCES)
    magic_enum_add_std_reflection_options(${test_name}-cpp26-reflection)
    target_compile_definitions(${test_name}-cpp26-reflection PRIVATE MAGIC_ENUM_TEST_STD_REFLECTION=1)
  endforeach()
  target_compile_definitions(test-cpp26-reflection PRIVATE MAGIC_ENUM_ENABLE_HASH=1)

  set(reflection_test test_std_reflection-auto-cpp26)
  magic_enum_make_test(test_reflection.cpp ${reflection_test} ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
  magic_enum_add_std_reflection_options(${reflection_test})
  target_compile_definitions(${reflection_test} PRIVATE MAGIC_ENUM_TEST_STD_REFLECTION=1)

  set(reflection_test test_std_reflection-force-compiler-specific-cpp26)
  magic_enum_make_test(test_reflection.cpp ${reflection_test} ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
  magic_enum_add_std_reflection_options(${reflection_test})
  target_compile_definitions(${reflection_test} PRIVATE MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION=1)

  foreach(character_width IN ITEMS 8 16 32)
    set(character_test test_character_type-char${character_width}-std-reflection-cpp26)
    magic_enum_make_test(test_character_types.cpp ${character_test} ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
    magic_enum_add_std_reflection_options(${character_test})
    target_compile_definitions(${character_test} PRIVATE
      MAGIC_ENUM_TEST_CHARACTER_WIDTH=${character_width}
      MAGIC_ENUM_TEST_STD_REFLECTION=1
    )

    set(character_test test_character_type-char${character_width}-force-compiler-specific-cpp26)
    magic_enum_make_test(test_character_types.cpp ${character_test} ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
    magic_enum_add_std_reflection_options(${character_test})
    target_compile_definitions(${character_test} PRIVATE
      MAGIC_ENUM_TEST_CHARACTER_WIDTH=${character_width}
      MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION=1
    )
  endforeach()

  magic_enum_make_compile_fail_test(
    synthetic-value
    1
    "magic_enum::customize::enum_name cannot add an undeclared enum value"
  )
  magic_enum_make_compile_fail_test(
    incomplete-enum
    4
    "magic_enum requires a complete enum definition"
  )
  magic_enum_make_compile_fail_test(
    invalid-prefix-equal-standard
    2
    "magic_enum::customize requires valid prefix length"
  )
  magic_enum_make_compile_fail_test(
    invalid-prefix-greater-standard
    3
    "magic_enum::customize requires valid prefix length"
  )
  magic_enum_make_compile_fail_test(
    invalid-prefix-equal-compiler-specific
    2
    "magic_enum::customize requires valid prefix length"
    MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION=1
  )
  magic_enum_make_compile_fail_test(
    invalid-prefix-greater-compiler-specific
    3
    "magic_enum::customize requires valid prefix length"
    MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION=1
  )
  magic_enum_make_compile_fail_test(
    nonstandard-character-type
    5
    "magic_enum standard reflection requires string_view::value_type to be a standard character type"
  )
endif()

if(MAGIC_ENUM_USE_MODULES)
  magic_enum_add_module_tests(cpp${MAGIC_ENUM_MODULE_CXX_STANDARD})
endif()

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
      "-DCMAKE_CXX_EXTENSIONS=OFF"
      "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
      "-DCMAKE_PREFIX_PATH=${MAGIC_ENUM_INSTALLED_CONSUMER_PREFIX_PATH}"
      "-DMAGIC_ENUM_EXPECT_IMPORT_STD=${MAGIC_ENUM_MODULE_IMPORT_STD}"
      "-DMAGIC_ENUM_EXPECT_FMT=${MAGIC_ENUM_MODULE_WITH_FMT}"
  )
  if(CMAKE_CXX_STANDARD)
    list(APPEND MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_OPTIONS
         "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
    )
    if(DEFINED CMAKE_CXX_STANDARD_REQUIRED)
      list(APPEND MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_OPTIONS
           "-DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED}"
      )
    endif()
  endif()
  if(WIN32)
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
  if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION AND
     MAGIC_ENUM_MODULE_CXX_STANDARD GREATER_EQUAL 26)
    list(APPEND MAGIC_ENUM_INSTALLED_CONSUMER_BUILD_OPTIONS
         "-DMAGIC_ENUM_EXPECT_STD_REFLECTION=ON"
    )
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
