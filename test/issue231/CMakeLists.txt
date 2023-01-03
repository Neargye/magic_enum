cmake_minimum_required(VERSION 3.15)
project (mve LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)


add_library(enum STATIC enum.cpp)
target_link_libraries(enum PRIVATE ${CMAKE_PROJECT_NAME})


make_test(issue231_no_range.cpp issue231_no_range c++17)
target_link_libraries(issue231_no_range PRIVATE enum)
target_include_directories(issue231_no_range PRIVATE ../3rdparty/Catch2/include)

make_test(issue231_with_range.cpp issue231_with_range c++17)
target_link_libraries(issue231_with_range PRIVATE enum)
target_include_directories(issue231_with_range PRIVATE ../3rdparty/Catch2/include)

