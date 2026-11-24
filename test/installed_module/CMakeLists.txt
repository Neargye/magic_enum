cmake_minimum_required(VERSION 3.28...4.5)

project(magic_enum_installed_module_test LANGUAGES CXX)

find_package(magic_enum CONFIG REQUIRED)

add_executable(magic_enum_installed_module_test main.cpp)
target_link_libraries(magic_enum_installed_module_test PRIVATE magic_enum::magic_enum_module)
set_target_properties(
  magic_enum_installed_module_test
  PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_SCAN_FOR_MODULES ON
)

if(MAGIC_ENUM_EXPECT_IMPORT_STD)
  # CMake 4.5 removed CXX_MODULE_STD.
  if(CMAKE_VERSION VERSION_LESS "4.5")
    set_property(TARGET magic_enum_installed_module_test PROPERTY CXX_MODULE_STD ON)
  endif()
  target_compile_definitions(
    magic_enum_installed_module_test PRIVATE MAGIC_ENUM_TEST_IMPORT_STD
  )
endif()

if(MAGIC_ENUM_EXPECT_STD_REFLECTION)
  target_compile_definitions(
    magic_enum_installed_module_test PRIVATE MAGIC_ENUM_TEST_STD_REFLECTION=1
  )
endif()

if(MAGIC_ENUM_EXPECT_FMT)
  get_target_property(magic_enum_module_links magic_enum::magic_enum_module INTERFACE_LINK_LIBRARIES)
  if(NOT MAGIC_ENUM_EXPECT_FMT_TARGET IN_LIST magic_enum_module_links)
    message(FATAL_ERROR "magic_enum::magic_enum_module links ${magic_enum_module_links}, expected ${MAGIC_ENUM_EXPECT_FMT_TARGET}")
  endif()
  target_compile_definitions(magic_enum_installed_module_test PRIVATE MAGIC_ENUM_TEST_FMT)
endif()

enable_testing()
add_test(NAME magic_enum_installed_module_test COMMAND magic_enum_installed_module_test)
