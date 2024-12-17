cmake_minimum_required(VERSION 3.16)
project(bitstream)

set(CMAKE_CXX_STANDARD 20)

#add_subdirectory(third_party/googletest)
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} include)
include_directories(include)

add_executable(test tests/test.cpp)
target_link_libraries(test gtest gtest_main)
