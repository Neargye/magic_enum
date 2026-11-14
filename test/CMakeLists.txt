include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION AND NOT TARGET magic_enum::magic_enum)
  find_package(magic_enum CONFIG REQUIRED)
endif()

if(MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION_PKGCONFIG)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(magic_enum REQUIRED IMPORTED_TARGET magic_enum)
  set(MAGIC_ENUM_TEST_LIBRARY PkgConfig::magic_enum)
else()
  set(MAGIC_ENUM_TEST_LIBRARY magic_enum::magic_enum)
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
  set(MAGIC_ENUM_TEST_COMPILE_OPTIONS -Wall -Wextra -Wshadow -pedantic-errors -Werror ${MAGIC_ENUM_STRICT_WARNINGS})
  set(MAGIC_ENUM_TEST_FLAG_PREFIX -std=)

  # Clang 22+ treats __COUNTER__ as a C2y extension, but doctest uses it.
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "22.0")
    list(APPEND MAGIC_ENUM_TEST_COMPILE_OPTIONS -Wno-c2y-extensions)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "NVHPC")
  set(MAGIC_ENUM_TEST_FLAG_PREFIX -std=)
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
    static_assert(std::meta::identifier_of(enumerators[0]).size() == 5);
    int main() {}
  ]])
  set(MAGIC_ENUM_SAVED_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${MAGIC_ENUM_TEST_FLAG_PREFIX}${MAGIC_ENUM_CPP26_STANDARD_FLAG}")
  check_cxx_source_compiles(
    "${MAGIC_ENUM_STD_REFLECTION_PROBE}"
    MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_BY_DEFAULT
  )

  if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_BY_DEFAULT)
    set(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION TRUE)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Prefer reflection provided by the active C++26 mode. Use GNU's temporary opt-in only when the default probe fails.
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -freflection")
    check_cxx_source_compiles(
      "${MAGIC_ENUM_STD_REFLECTION_PROBE}"
      MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_WITH_GNU_OPTION
    )
    if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_WITH_GNU_OPTION)
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

add_library(magic_enum_test_options INTERFACE)
target_compile_options(magic_enum_test_options INTERFACE ${MAGIC_ENUM_TEST_COMPILE_OPTIONS})
# CI sets this so the fmt tests fail instead of being skipped when <fmt/format.h> is missing.
if(MAGIC_ENUM_TEST_REQUIRE_FMT)
  target_compile_definitions(magic_enum_test_options INTERFACE MAGIC_ENUM_TEST_REQUIRE_FMT)
endif()

function(magic_enum_make_test sources target standard_flag)
  add_executable(${target} ${sources})
  set(test_targets ${target})

  list(GET sources 0 primary_source)
  get_filename_component(test_name "${primary_source}" NAME_WE)
  if(test_name IN_LIST MAGIC_ENUM_DOCTEST_SOURCES)
    string(REPLACE "c++" "cpp" doctest_suffix "${standard_flag}")
    set(doctest_target test_doctest-${doctest_suffix})
    if(NOT TARGET ${doctest_target})
      add_library(${doctest_target} OBJECT test_main.cpp)
      target_link_libraries(${doctest_target} PRIVATE magic_enum_test_options)
      target_compile_definitions(${doctest_target} PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
      target_include_directories(${doctest_target} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
    endif()
    list(APPEND test_targets ${doctest_target})
    target_link_libraries(${target} PRIVATE ${doctest_target})
  endif()

  set_target_properties(${test_targets} PROPERTIES CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
  if(standard_flag MATCHES "^c\\+\\+(17|20|23)$")
    set_target_properties(${test_targets} PROPERTIES CXX_STANDARD "${CMAKE_MATCH_1}" CXX_STANDARD_REQUIRED ON)
  else()
    foreach(test_target IN LISTS test_targets)
      target_compile_options(${test_target} PRIVATE ${MAGIC_ENUM_TEST_FLAG_PREFIX}${standard_flag})
    endforeach()
  endif()

  target_link_libraries(${target} PRIVATE magic_enum_test_options)
  target_link_libraries(${target} PRIVATE ${MAGIC_ENUM_TEST_LIBRARY})
  add_test(NAME ${target} COMMAND ${target})
endfunction()

function(magic_enum_make_compile_fail_test category case_name case_id expected_text)
  if(category STREQUAL "reflection")
    set(suffix cpp26)
  else()
    set(suffix cpp17)
  endif()
  set(target test_${category}-compile-fail-${case_name}-${suffix})
  add_executable(${target} EXCLUDE_FROM_ALL test_${category}_compile_fail.cpp)
  target_link_libraries(${target} PRIVATE magic_enum_test_options)
  if(category STREQUAL "reflection")
    target_compile_options(${target} PRIVATE
      ${MAGIC_ENUM_TEST_FLAG_PREFIX}${MAGIC_ENUM_CPP26_STANDARD_FLAG}
    )
    magic_enum_add_std_reflection_options(${target})
  else()
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
  endif()
  string(TOUPPER "${category}" case_prefix)
  target_compile_definitions(${target} PRIVATE MAGIC_ENUM_${case_prefix}_COMPILE_FAIL_CASE=${case_id} ${ARGN})
  target_link_libraries(${target} PRIVATE ${MAGIC_ENUM_TEST_LIBRARY})
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)

  set(compile_commands)
  if(CMAKE_GENERATOR MATCHES "Ninja|Makefiles" AND NOT CMAKE_CONFIGURATION_TYPES)
    # Reuse CMake's exact compiler flags without concurrent builds of the same tree.
    set_target_properties(${target} PROPERTIES EXPORT_COMPILE_COMMANDS ON)
    set(compile_commands "${CMAKE_BINARY_DIR}/compile_commands.json")
  endif()
  add_test(
    NAME ${target}
    COMMAND ${CMAKE_COMMAND}
      "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
      "-DCOMPILE_COMMANDS=${compile_commands}"
      "-DTARGET_NAME=${target}"
      "-DCONFIG=$<CONFIG>"
      "-DEXPECTED_TEXT=${expected_text}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/check_compile_fail.cmake"
  )
  if(NOT compile_commands)
    set_tests_properties(${target} PROPERTIES RUN_SERIAL TRUE)
  endif()
endfunction()

function(magic_enum_make_module_test src target module_target)
  add_executable(${target} ${src})
  target_link_libraries(${target} PRIVATE magic_enum_test_options)
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

set(MAGIC_ENUM_TEST_SOURCES
  test test_flags test_switch test_utility test_operators test_format
  test_aliases test_containers test_wchar_t
)
set(MAGIC_ENUM_TEST_EXTRA_SOURCES_test test_global_names.cpp)
set(MAGIC_ENUM_TEST_EXTRA_SOURCES_test_operators test_operators_other.cpp)
# Suites with name lookup or switch paths that MAGIC_ENUM_ENABLE_HASH changes.
set(MAGIC_ENUM_TEST_HASH_SOURCES test test_flags test_switch test_operators)
set(MAGIC_ENUM_DOCTEST_SOURCES ${MAGIC_ENUM_TEST_SOURCES} test_range test_reflection test_no_named_zero_in_flags test_no_check_out_of_range)

# nvc++ cannot reflect enums declared inside functions, which the other suites use.
if(CMAKE_CXX_COMPILER_ID STREQUAL "NVHPC")
  list(APPEND MAGIC_ENUM_DOCTEST_SOURCES test_nvcc)
  magic_enum_make_test(test_nvcc.cpp test_nvcc-cpp17 c++17)
  if(MAGIC_ENUM_HAS_CPP20_FLAG)
    magic_enum_make_test(test_nvcc.cpp test_nvcc-cpp20 c++20)
  endif()
  return()
endif()

function(magic_enum_add_tests suffix standard_flag)
  foreach(test_name IN LISTS MAGIC_ENUM_TEST_SOURCES)
    set(test_sources ${test_name}.cpp ${MAGIC_ENUM_TEST_EXTRA_SOURCES_${test_name}})
    magic_enum_make_test("${test_sources}" ${test_name}-${suffix} ${standard_flag})
  endforeach()
endfunction()


magic_enum_add_tests(cpp17 c++17)
magic_enum_make_test("test_operators.cpp;${MAGIC_ENUM_TEST_EXTRA_SOURCES_test_operators}" test_operators-no-auto-cpp17 c++17)
target_compile_definitions(test_operators-no-auto-cpp17 PRIVATE MAGIC_ENUM_NO_AUTO_IS_FLAGS MAGIC_ENUM_TEST_NO_AUTO_IS_FLAGS)
magic_enum_make_test(test_no_named_zero_in_flags.cpp test_no_named_zero_in_flags-cpp17 c++17)
target_compile_definitions(test_no_named_zero_in_flags-cpp17 PRIVATE MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS)
magic_enum_make_test(test_no_check_out_of_range.cpp test_no_check_out_of_range-cpp17 c++17)
target_compile_definitions(test_no_check_out_of_range-cpp17 PRIVATE MAGIC_ENUM_NO_CHECK_OUT_OF_RANGE)
set(MAGIC_ENUM_FORMAT_TEST_STANDARD c++17)
if(MAGIC_ENUM_HAS_CPP20_FLAG)
  set(MAGIC_ENUM_FORMAT_TEST_STANDARD c++20)
endif()
foreach(format_mode IN ITEMS no-std no-fmt no-format)
  set(format_target test_format-${format_mode})
  magic_enum_make_test(test_format.cpp ${format_target} ${MAGIC_ENUM_FORMAT_TEST_STANDARD})
  if(NOT format_mode STREQUAL "no-fmt")
    target_compile_definitions(${format_target} PRIVATE MAGIC_ENUM_NO_STD_FORMAT)
  endif()
  if(NOT format_mode STREQUAL "no-std")
    target_compile_definitions(${format_target} PRIVATE MAGIC_ENUM_NO_FMT_FORMAT)
  endif()
endforeach()
magic_enum_make_test(test_range.cpp test_range-cpp17 c++17)
foreach(hook_form RANGE 1 4)
  magic_enum_make_compile_fail_test(configuration invalid-hook-${hook_form} 20
    "magic_enum customization hook must return a non-final class by value"
    MAGIC_ENUM_TEST_ENUM_FORM=${hook_form})
endforeach()
foreach(test_name IN LISTS MAGIC_ENUM_TEST_HASH_SOURCES)
  set(test_sources ${test_name}.cpp ${MAGIC_ENUM_TEST_EXTRA_SOURCES_${test_name}})
  magic_enum_make_test("${test_sources}" ${test_name}-hash-cpp17 c++17)
  target_compile_definitions(${test_name}-hash-cpp17 PRIVATE MAGIC_ENUM_ENABLE_HASH=1)
endforeach()
magic_enum_make_test(test_fuse.cpp test-no-typesafe-enum-fuse-cpp17 c++17)
target_compile_definitions(test-no-typesafe-enum-fuse-cpp17 PRIVATE MAGIC_ENUM_NO_TYPESAFE_ENUM_FUSE=1)
magic_enum_make_test(test_fuse.cpp test_fuse-cpp17 c++17)
magic_enum_make_test("test_shared_config.cpp;test_shared_config_other.cpp" test_shared_config-cpp17 c++17)
magic_enum_make_test("test_shared_config.cpp;test_shared_config_other.cpp" test_shared_config-hash-cpp17 c++17)
target_compile_definitions(test_shared_config-hash-cpp17 PRIVATE MAGIC_ENUM_ENABLE_HASH=1)
magic_enum_make_test(test_containers_no_exceptions.cpp test_containers_no_exceptions-cpp17 c++17)
if(MSVC)
  target_compile_definitions(test_containers_no_exceptions-cpp17 PRIVATE MAGIC_ENUM_NO_EXCEPTION)
else()
  target_compile_options(test_containers_no_exceptions-cpp17 PRIVATE -fno-exceptions)
endif()
foreach(case_name IN ITEMS set-key set-plain-key from-mask bitset-mask bitset-name bitset-raw-number bitset-raw-string bitset-raw-position array-at bitset-zero bitset-combination)
  add_test(NAME test_containers_no_exceptions-${case_name}-cpp17 COMMAND test_containers_no_exceptions-cpp17 ${case_name})
  set_tests_properties(test_containers_no_exceptions-${case_name}-cpp17 PROPERTIES PASS_REGULAR_EXPRESSION "magic_enum::containers aborted")
endforeach()
# GCC supports UTF-8 identifiers since version 10.
if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10))
  magic_enum_make_test(test_nonascii.cpp test_nonascii-cpp17 c++17)
  if(MSVC)
    target_compile_options(test_nonascii-cpp17 PRIVATE /utf-8)
  endif()
endif()
magic_enum_make_compile_fail_test(
  configuration
  required-std-reflection
  23
  "MAGIC_ENUM_REQUIRE_STD_REFLECTION requires C++26 standard reflection support"
)
magic_enum_make_compile_fail_test(
  configuration
  conflicting-reflection
  24
  "MAGIC_ENUM_REQUIRE_STD_REFLECTION and MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION cannot be used together"
)
magic_enum_make_compile_fail_test(
  configuration
  unnamed-type-name
  25
  "magic_enum::enum_type_name enum type does not have a name"
)
magic_enum_make_compile_fail_test(
  configuration
  mixed-header-versions
  26
  "magic_enum headers of different versions are mixed"
)
magic_enum_make_compile_fail_test(
  configuration
  index-no-index
  27
  "magic_enum::enum_index enum value has no index"
)
magic_enum_make_compile_fail_test(
  configuration
  name-no-name
  28
  "magic_enum::enum_name enum value does not have a name"
)
magic_enum_make_compile_fail_test(
  configuration
  value-out-of-range
  29
  "magic_enum::enum_value out of range"
)
magic_enum_make_compile_fail_test(
  configuration
  min-empty
  30
  "magic_enum::enum_min requires at least one reflected enum value"
  MAGIC_ENUM_TEST_ENUM_FORM=1
  MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
)
magic_enum_make_compile_fail_test(
  configuration
  max-empty
  30
  "magic_enum::enum_max requires at least one reflected enum value"
  MAGIC_ENUM_TEST_ENUM_FORM=2
  MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
)
foreach(multibit_form RANGE 1 6)
  magic_enum_make_compile_fail_test(
    configuration
    auto-flags-multibit-${multibit_form}
    31
    "magic_enum detected flags from operator| and operator&, but a named value has more than one bit set"
    MAGIC_ENUM_TEST_ENUM_FORM=${multibit_form}
  )
endforeach()
magic_enum_make_compile_fail_test(
  configuration
  conflicting-assert
  1
  "MAGIC_ENUM_NO_ASSERT and MAGIC_ENUM_ASSERT cannot be used together"
)
magic_enum_make_compile_fail_test(
  configuration
  invalid-is-flags
  2
  "magic_enum::customize::enum_range requires is_flags to have bool type"
)
foreach(range_offset IN ITEMS 1 2 4 8)
  foreach(range_side IN ITEMS below above)
    if(range_side STREQUAL "below")
      set(signed_range_offset -${range_offset})
    else()
      set(signed_range_offset ${range_offset})
    endif()
    magic_enum_make_compile_fail_test(
      configuration
      range-${range_side}-${range_offset}
      3
      "magic_enum::enum_range found a named value outside the range"
      MAGIC_ENUM_TEST_RANGE_OFFSET=${signed_range_offset}
    )
  endforeach()
endforeach()
magic_enum_make_compile_fail_test(
  configuration
  range-renamed-above-4
  3
  "magic_enum::enum_range found a named value outside the range"
  MAGIC_ENUM_TEST_RANGE_OFFSET=4
  MAGIC_ENUM_TEST_RANGE_RENAMED
)
foreach(range_case IN ITEMS below-1:-1 above-8:8)
  string(REPLACE ":" ";" range_case "${range_case}")
  list(GET range_case 0 range_name)
  list(GET range_case 1 range_offset)
  magic_enum_make_compile_fail_test(
    configuration
    range-unfixed-${range_name}
    3
    "magic_enum::enum_range found a named value outside the range"
    MAGIC_ENUM_TEST_RANGE_OFFSET=${range_offset}
    MAGIC_ENUM_TEST_RANGE_UNFIXED
  )
endforeach()
magic_enum_make_compile_fail_test(
  configuration
  synthetic-value
  21
  "magic_enum::customize::enum_name cannot add an undeclared enum value"
)
magic_enum_make_compile_fail_test(
  configuration
  custom-zero-undeclared
  22
  "magic_enum::customize::enum_name cannot add an undeclared enum value"
)
magic_enum_make_compile_fail_test(
  configuration
  custom-zero-undeclared-flags
  22
  "magic_enum::customize::enum_name cannot add an undeclared enum value"
  MAGIC_ENUM_TEST_FLAGS
)
magic_enum_make_compile_fail_test(
  configuration
  range-no-check-reflected-enum
  3
  "magic_enum::enum_range found a named value outside the range"
  MAGIC_ENUM_TEST_RANGE_OFFSET=2
  MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
)
magic_enum_make_compile_fail_test(
  configuration
  range-default
  4
  "magic_enum::enum_range found a named value outside the range"
)
magic_enum_make_compile_fail_test(
  configuration
  range-below-int-min
  3
  "magic_enum::enum_range found a named value outside the range"
  "MAGIC_ENUM_TEST_RANGE_BOUNDARY=(std::numeric_limits<int>::min)()"
  MAGIC_ENUM_TEST_RANGE_OFFSET=-1
)
magic_enum_make_compile_fail_test(
  configuration
  range-above-int-max
  3
  "magic_enum::enum_range found a named value outside the range"
  "MAGIC_ENUM_TEST_RANGE_BOUNDARY=(std::numeric_limits<int>::max)()"
  MAGIC_ENUM_TEST_RANGE_OFFSET=1
)
magic_enum_make_compile_fail_test(
  configuration
  invalid-is-flags-as-common
  2
  "magic_enum::customize::enum_range requires is_flags to have bool type"
  MAGIC_ENUM_TEST_ENUM_FORM=1
)
magic_enum_make_compile_fail_test(
  configuration
  invalid-is-flags-as-flags
  2
  "magic_enum::customize::enum_range requires is_flags to have bool type"
  MAGIC_ENUM_TEST_ENUM_FORM=2
)
magic_enum_make_compile_fail_test(
  configuration
  invalid-is-flags-switch
  2
  "magic_enum::customize::enum_range requires is_flags to have bool type"
  MAGIC_ENUM_TEST_ENUM_FORM=3
)
magic_enum_make_compile_fail_test(
  configuration
  invalid-is-flags-switch-fallback
  2
  "magic_enum::customize::enum_range requires is_flags to have bool type"
  MAGIC_ENUM_TEST_ENUM_FORM=4
)

magic_enum_make_compile_fail_test(
  configuration
  name-forward-scoped
  5
  "magic_enum::enum_name requires a non-empty reflected enum."
  MAGIC_ENUM_TEST_ENUM_FORM=1
)
magic_enum_make_compile_fail_test(
  configuration
  name-forward-scoped-underlying
  5
  "magic_enum::enum_name requires a non-empty reflected enum."
  MAGIC_ENUM_TEST_ENUM_FORM=2
)
magic_enum_make_compile_fail_test(
  configuration
  name-forward-unscoped
  5
  "magic_enum::enum_name requires a non-empty reflected enum."
  MAGIC_ENUM_TEST_ENUM_FORM=3
)
magic_enum_make_compile_fail_test(
  configuration
  name-empty
  5
  "magic_enum::enum_name requires a non-empty reflected enum."
  MAGIC_ENUM_TEST_ENUM_FORM=4
)
magic_enum_make_compile_fail_test(
  configuration
  fuse-empty
  7
  "magic_enum::enum_fuse requires a non-empty reflected enum."
)
magic_enum_make_compile_fail_test(
  configuration
  fuse-empty-no-check
  7
  "magic_enum::enum_fuse requires a non-empty reflected enum."
  MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
)
magic_enum_make_compile_fail_test(
  configuration
  values-empty
  8
  "magic_enum::enum_values requires a non-empty reflected enum."
  MAGIC_ENUM_TEST_ENUM_FORM=1
)
magic_enum_make_compile_fail_test(
  configuration
  values-outside-range
  8
  "magic_enum::enum_values requires a non-empty reflected enum."
  MAGIC_ENUM_TEST_ENUM_FORM=2
)
magic_enum_make_compile_fail_test(
  configuration
  next-circular-empty
  19
  "magic_enum::enum_next_value_circular requires a non-empty reflected enum."
  MAGIC_ENUM_TEST_ENUM_FORM=1
)
magic_enum_make_compile_fail_test(
  configuration
  prev-circular-empty
  19
  "magic_enum::enum_prev_value_circular requires a non-empty reflected enum."
  MAGIC_ENUM_TEST_ENUM_FORM=2
)
magic_enum_make_compile_fail_test(
  configuration
  custom-name-empty
  9
  "magic_enum::customize::enum_name requires a non-empty name."
)
magic_enum_make_compile_fail_test(
  configuration
  custom-name-empty-fallback
  9
  "magic_enum::customize::enum_name requires a non-empty name."
  MAGIC_ENUM_TEST_FORCE_FALLBACK
)
magic_enum_make_compile_fail_test(
  configuration
  range-int-span
  10
  "magic_enum::enum_range requires max - min to be less than UINT16_MAX."
  MAGIC_ENUM_TEST_ENUM_FORM=1
)
magic_enum_make_compile_fail_test(
  configuration
  range-outside-int
  10
  "magic_enum::enum_range requires min and max to fit in int."
  MAGIC_ENUM_TEST_ENUM_FORM=2
)
magic_enum_make_compile_fail_test(
  configuration
  range-min-max
  10
  "magic_enum::enum_range requires min <= max."
  MAGIC_ENUM_TEST_ENUM_FORM=3
)
magic_enum_make_compile_fail_test(
  configuration
  reflected-range-outside-int
  10
  "magic_enum::enum_range requires min and max to fit in int."
  MAGIC_ENUM_TEST_ENUM_FORM=2
  MAGIC_ENUM_TEST_REFLECTED
)
magic_enum_make_compile_fail_test(
  configuration
  reflected-range-min-max
  10
  "magic_enum::enum_range requires min <= max."
  MAGIC_ENUM_TEST_ENUM_FORM=3
  MAGIC_ENUM_TEST_REFLECTED
)
magic_enum_make_compile_fail_test(
  configuration
  for-each-rvalue-only
  11
  "magic_enum::enum_for_each requires the callable to be invocable with every enum value."
  MAGIC_ENUM_TEST_ENUM_FORM=1
)
magic_enum_make_compile_fail_test(
  configuration
  for-each-partial
  11
  "magic_enum::enum_for_each requires the callable to be invocable with every enum value."
  MAGIC_ENUM_TEST_ENUM_FORM=2
)
magic_enum_make_compile_fail_test(
  configuration
  bitwise-macro-non-enum
  14
  "magic_enum bitwise operators require an enum type."
)
magic_enum_make_compile_fail_test(
  configuration
  iostream-macro-non-enum
  15
  "magic_enum iostream operators require an enum type."
)
magic_enum_make_compile_fail_test(
  configuration
  flags-name-string-value-type
  16
  "magic_enum::enum_flags_name requires String::value_type to be magic_enum::char_type."
)
magic_enum_make_compile_fail_test(
  configuration
  bitset-to-string-value-type
  17
  "magic_enum::containers::bitset::to_string requires String::value_type to be magic_enum::char_type."
)
magic_enum_make_compile_fail_test(
  configuration
  set-empty
  18
  "magic_enum::containers::set requires a non-empty reflected enum."
  MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
)
magic_enum_make_compile_fail_test(
  configuration
  bitset-only-zero
  13
  "magic_enum::containers::bitset requires at least one flag bit, a named zero has no bit."
)
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  magic_enum_make_compile_fail_test(
    configuration
    pretty-function-macro
    12
    "magic_enum requires the builtin __PRETTY_FUNCTION__, not a macro."
  )
endif()
magic_enum_make_test(test_resharper.cpp test_resharper-cpp17 c++17)
if(MAGIC_ENUM_HAS_CPP20_FLAG)
  magic_enum_make_test(test_resharper.cpp test_resharper-cpp20 c++20)
endif()

magic_enum_make_test(test_configuration_compile_fail.cpp test_enum_count-cpp17 c++17)
target_compile_definitions(test_enum_count-cpp17 PRIVATE MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE=6)
magic_enum_make_test(test_configuration_compile_fail.cpp test_enum_count-no-check-cpp17 c++17)
target_compile_definitions(test_enum_count-no-check-cpp17 PRIVATE
  MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE=6
  MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
)
magic_enum_make_test(test_configuration_compile_fail.cpp test_enum_count-no-check-hash-cpp17 c++17)
target_compile_definitions(test_enum_count-no-check-hash-cpp17 PRIVATE
  MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE=6
  MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
  MAGIC_ENUM_ENABLE_HASH
)

if(MAGIC_ENUM_HAS_CPP20_FLAG)
  magic_enum_make_test(test_configuration_compile_fail.cpp test_enum_count-no-check-cpp20 c++20)
  target_compile_definitions(test_enum_count-no-check-cpp20 PRIVATE
    MAGIC_ENUM_CONFIGURATION_COMPILE_FAIL_CASE=6
    MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM
  )
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
  if(MAGIC_ENUM_TEST_STANDARD EQUAL 26 AND MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION_BY_DEFAULT)
    foreach(test_name IN LISTS MAGIC_ENUM_TEST_SOURCES)
      target_compile_definitions(${test_name}-${MAGIC_ENUM_TEST_SUFFIX} PRIVATE MAGIC_ENUM_TEST_STD_REFLECTION=1 MAGIC_ENUM_REQUIRE_STD_REFLECTION)
    endforeach()
  endif()
endforeach()

if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION)
  magic_enum_make_test("test_operators.cpp;${MAGIC_ENUM_TEST_EXTRA_SOURCES_test_operators}" test_operators-no-auto-cpp26-reflection ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
  magic_enum_add_std_reflection_options(test_operators-no-auto-cpp26-reflection)
  target_compile_definitions(test_operators-no-auto-cpp26-reflection PRIVATE
    MAGIC_ENUM_NO_AUTO_IS_FLAGS MAGIC_ENUM_TEST_NO_AUTO_IS_FLAGS MAGIC_ENUM_TEST_STD_REFLECTION=1 MAGIC_ENUM_REQUIRE_STD_REFLECTION)
  magic_enum_make_test(test_no_named_zero_in_flags.cpp test_no_named_zero_in_flags-cpp26-reflection ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
  magic_enum_add_std_reflection_options(test_no_named_zero_in_flags-cpp26-reflection)
  target_compile_definitions(test_no_named_zero_in_flags-cpp26-reflection PRIVATE
    MAGIC_ENUM_NO_NAMED_ZERO_IN_FLAGS MAGIC_ENUM_TEST_STD_REFLECTION=1 MAGIC_ENUM_REQUIRE_STD_REFLECTION)
  magic_enum_make_test(test_nonascii.cpp test_nonascii-cpp26-reflection ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
  magic_enum_add_std_reflection_options(test_nonascii-cpp26-reflection)
  target_compile_definitions(test_nonascii-cpp26-reflection PRIVATE MAGIC_ENUM_REQUIRE_STD_REFLECTION)
  magic_enum_add_tests(cpp26-reflection ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
  foreach(test_name IN LISTS MAGIC_ENUM_TEST_SOURCES)
    magic_enum_add_std_reflection_options(${test_name}-cpp26-reflection)
    target_compile_definitions(${test_name}-cpp26-reflection PRIVATE MAGIC_ENUM_TEST_STD_REFLECTION=1 MAGIC_ENUM_REQUIRE_STD_REFLECTION)
  endforeach()
  foreach(test_name IN LISTS MAGIC_ENUM_TEST_HASH_SOURCES)
    target_compile_definitions(${test_name}-cpp26-reflection PRIVATE MAGIC_ENUM_ENABLE_HASH=1)
  endforeach()

  magic_enum_add_tests(cpp26-force-compiler-specific ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
  foreach(test_name IN LISTS MAGIC_ENUM_TEST_SOURCES)
    magic_enum_add_std_reflection_options(${test_name}-cpp26-force-compiler-specific)
    target_compile_definitions(
      ${test_name}-cpp26-force-compiler-specific
      PRIVATE MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION=1
    )
  endforeach()

  foreach(backend IN ITEMS std-reflection force-compiler-specific)
    if(backend STREQUAL "std-reflection")
      set(backend_definition MAGIC_ENUM_TEST_STD_REFLECTION=1 MAGIC_ENUM_REQUIRE_STD_REFLECTION)
      set(reflection_test test_std_reflection-auto-cpp26)
    else()
      set(backend_definition MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION=1)
      set(reflection_test test_std_reflection-${backend}-cpp26)
    endif()
    magic_enum_make_test(test_reflection.cpp ${reflection_test} ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
    magic_enum_add_std_reflection_options(${reflection_test})
    target_compile_definitions(${reflection_test} PRIVATE ${backend_definition})

    foreach(character_width IN ITEMS 8 16 32)
      set(character_test test_character_type-char${character_width}-${backend}-cpp26)
      magic_enum_make_test(test_character_types.cpp ${character_test} ${MAGIC_ENUM_CPP26_STANDARD_FLAG})
      magic_enum_add_std_reflection_options(${character_test})
      target_compile_definitions(${character_test} PRIVATE
        MAGIC_ENUM_TEST_CHARACTER_WIDTH=${character_width} ${backend_definition})
    endforeach()
  endforeach()

  magic_enum_make_compile_fail_test(
    reflection
    synthetic-value
    1
    "magic_enum::customize::enum_name cannot add an undeclared enum value"
  )
  magic_enum_make_compile_fail_test(
    reflection
    incomplete-enum
    4
    "magic_enum requires a complete enum definition"
  )
  magic_enum_make_compile_fail_test(
    reflection
    auto-flags-multibit
    9
    "magic_enum detected flags from operator| and operator&, but a named value has more than one bit set"
  )
  foreach(backend IN ITEMS standard compiler-specific)
    set(customization_definitions)
    if(backend STREQUAL "compiler-specific")
      list(APPEND customization_definitions MAGIC_ENUM_FORCE_COMPILER_SPECIFIC_REFLECTION=1)
    endif()
    magic_enum_make_compile_fail_test(
      reflection
      invalid-prefix-equal-${backend}
      2
      "magic_enum::customize requires valid prefix length"
      ${customization_definitions}
    )
    magic_enum_make_compile_fail_test(
      reflection
      invalid-prefix-greater-${backend}
      3
      "magic_enum::customize requires valid prefix length"
      ${customization_definitions}
    )
    magic_enum_make_compile_fail_test(
      reflection
      custom-name-empty-${backend}
      6
      "magic_enum::customize::enum_name requires a non-empty name"
      ${customization_definitions}
    )
    magic_enum_make_compile_fail_test(
      reflection
      custom-zero-undeclared-${backend}
      8
      "magic_enum::customize::enum_name cannot add an undeclared enum value"
      ${customization_definitions}
    )
  endforeach()
  magic_enum_make_compile_fail_test(
    reflection
    nonstandard-character-type
    5
    "magic_enum standard reflection requires string_view::value_type to be a standard character type"
  )
endif()

if(MAGIC_ENUM_USE_MODULES)
  set(module_suffix cpp${MAGIC_ENUM_MODULE_CXX_STANDARD})
  magic_enum_make_module_test(test_module.cpp test_module-${module_suffix} magic_enum_module_test-${module_suffix})
  if(MAGIC_ENUM_MODULE_WITH_FMT)
    target_compile_definitions(test_module-${module_suffix} PRIVATE MAGIC_ENUM_TEST_FMT)
  endif()

  # Installed packages expose one module with the configuration fixed at build time.
  if(NOT MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION)
    magic_enum_make_module_library(magic_enum_module_test_no_auto-${module_suffix} MAGIC_ENUM_NO_AUTO_IS_FLAGS)
    magic_enum_make_module_test(test_module.cpp test_module-no-auto-${module_suffix} magic_enum_module_test_no_auto-${module_suffix})
    target_compile_definitions(test_module-no-auto-${module_suffix} PRIVATE MAGIC_ENUM_TEST_NO_AUTO_IS_FLAGS)
    if(MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION AND MAGIC_ENUM_MODULE_CXX_STANDARD GREATER_EQUAL 26)
      magic_enum_add_std_reflection_options(magic_enum_module_test_no_auto-${module_suffix})
    endif()
    add_library(magic_enum_test_types)
    set_target_properties(magic_enum_test_types PROPERTIES CXX_EXTENSIONS OFF)
    target_sources(magic_enum_test_types PUBLIC FILE_SET CXX_MODULES FILES test_module_types.cppm)
    target_compile_features(magic_enum_test_types PUBLIC cxx_std_${MAGIC_ENUM_MODULE_CXX_STANDARD})
    foreach(consumer IN ITEMS test_module-${module_suffix} test_module-no-auto-${module_suffix})
      target_link_libraries(${consumer} PRIVATE magic_enum_test_types)
      target_compile_definitions(${consumer} PRIVATE MAGIC_ENUM_TEST_EXPORTED_ENUM)
    endforeach()
    foreach(test_variant IN ITEMS _aliases _wchar_t)
      magic_enum_make_module_test(
        test_module${test_variant}.cpp
        test_module${test_variant}-${module_suffix}
        magic_enum_module_test${test_variant}-${module_suffix}
      )
    endforeach()
  endif()

  # CI builds this target and runs the tests labeled module.
  set(module_tests test_module-${module_suffix})
  # GCC 16 with -freflection rejects import std after a configuration file that includes standard headers.
  if(NOT MAGIC_ENUM_OPT_TEST_INSTALLED_VERSION AND
     NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MAGIC_ENUM_MODULE_IMPORT_STD AND
          MAGIC_ENUM_CAN_COMPILE_STD_REFLECTION AND MAGIC_ENUM_MODULE_CXX_STANDARD GREATER_EQUAL 26))
    list(APPEND module_tests test_module_aliases-${module_suffix} test_module_wchar_t-${module_suffix})
  endif()
  set_tests_properties(${module_tests} PROPERTIES LABELS module)
  add_custom_target(magic_enum_module_tests)
  add_dependencies(magic_enum_module_tests ${module_tests})
  foreach(module_target IN ITEMS magic_enum_module example_module_usage)
    if(TARGET ${module_target})
      add_dependencies(magic_enum_module_tests ${module_target})
    endif()
  endforeach()
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
  set_tests_properties(install-module-to-stagedir PROPERTIES FIXTURES_SETUP magic_enum_installed_module LABELS module)

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
  set_tests_properties(installed-module-consumer PROPERTIES FIXTURES_REQUIRED magic_enum_installed_module LABELS module)
endif()
