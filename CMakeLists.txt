cmake_minimum_required(VERSION 3.16)
project(bitstream)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

include_directories(include)

add_executable(test tests/test.cpp)
target_link_libraries(test gtest gtest_main)
