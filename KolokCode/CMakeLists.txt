cmake_minimum_required(VERSION 3.14)
project(SmallKolokvium LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(googletest)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/library
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

add_executable(run_tests
    tests/tests.cpp
    library/Solver.cpp
 )

target_link_libraries(run_tests PRIVATE gtest_main)

enable_testing()

include(GoogleTest)