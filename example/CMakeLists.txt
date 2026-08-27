include(CheckCXXCompilerFlag)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(MAGIC_ENUM_EXAMPLE_COMPILE_OPTIONS -Wall -Wextra -Wshadow -pedantic-errors -Werror)
elseif(MSVC)
  set(MAGIC_ENUM_EXAMPLE_COMPILE_OPTIONS /W4 /WX)
  check_cxx_compiler_flag(/permissive- MAGIC_ENUM_EXAMPLE_HAS_PERMISSIVE_FLAG)
  if(MAGIC_ENUM_EXAMPLE_HAS_PERMISSIVE_FLAG)
    list(APPEND MAGIC_ENUM_EXAMPLE_COMPILE_OPTIONS /permissive-)
  endif()
endif()

function(magic_enum_add_example target library standard)
  add_executable(${target} ${target}.cpp)
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
  target_compile_features(${target} PRIVATE cxx_std_${standard})
  target_compile_options(${target} PRIVATE ${MAGIC_ENUM_EXAMPLE_COMPILE_OPTIONS})
  target_link_libraries(${target} PRIVATE ${library})
endfunction()

set(MAGIC_ENUM_EXAMPLES
    example
    enum_flag_example
    example_containers_array
    example_containers_bitset
    example_containers_set
    example_containers_set_lookup
    example_custom_name
    example_switch
)

foreach(MAGIC_ENUM_EXAMPLE IN LISTS MAGIC_ENUM_EXAMPLES)
  magic_enum_add_example(${MAGIC_ENUM_EXAMPLE} magic_enum::magic_enum 17)
endforeach()

if(MAGIC_ENUM_USE_MODULES)
  magic_enum_add_example(
    example_module_usage
    magic_enum::magic_enum_module
    ${MAGIC_ENUM_MODULE_CXX_STANDARD}
  )
  set_target_properties(example_module_usage PROPERTIES CXX_SCAN_FOR_MODULES ON)
  if(MAGIC_ENUM_MODULE_IMPORT_STD)
    set_property(TARGET example_module_usage PROPERTY CXX_MODULE_STD ON)
    target_compile_definitions(example_module_usage PRIVATE MAGIC_ENUM_TEST_IMPORT_STD)
  endif()
endif()
