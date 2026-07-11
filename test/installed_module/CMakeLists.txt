cmake_minimum_required(VERSION 3.28)

project(magic_enum_installed_module_test LANGUAGES CXX)

find_package(magic_enum CONFIG REQUIRED)

add_executable(magic_enum_installed_module_test main.cpp)
target_link_libraries(magic_enum_installed_module_test PRIVATE magic_enum::magic_enum_module)
set_property(TARGET magic_enum_installed_module_test PROPERTY CXX_SCAN_FOR_MODULES ON)

if(MAGIC_ENUM_EXPECT_IMPORT_STD)
  set_property(TARGET magic_enum_installed_module_test PROPERTY CXX_MODULE_STD ON)
  target_compile_definitions(
    magic_enum_installed_module_test PRIVATE MAGIC_ENUM_TEST_IMPORT_STD
  )
endif()

if(MAGIC_ENUM_EXPECT_FMT)
  target_compile_definitions(magic_enum_installed_module_test PRIVATE MAGIC_ENUM_TEST_FMT)
endif()

enable_testing()
add_test(NAME magic_enum_installed_module_test COMMAND magic_enum_installed_module_test)
