add_executable(KolokCode2 main.cpp src/TaskService.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(KolokCode2 PRIVATE include)

add_link_options(
    $<$<CONFIG:Debug>:/NODEFAULTLIB:LIBCMTD>
    $<$<CONFIG:Release>:/NODEFAULTLIB:LIBCMT>
)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip 
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(tests tests/tests.cpp src/TaskService.cpp)

target_link_libraries(tests PRIVATE gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)