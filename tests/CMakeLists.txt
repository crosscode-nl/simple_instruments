cmake_minimum_required(VERSION 3.8.2)
project(simple_telemetry_tests LANGUAGES C CXX)

list(APPEND TEST_SRC
        main.cpp
        telemetry_tests.cpp
)

add_executable(simple_telemetry_tests ${TEST_SRC})
target_link_libraries(simple_telemetry_tests simple_telemetry)
target_include_directories(simple_telemetry_tests PUBLIC include)
target_compile_features(simple_telemetry_tests PUBLIC cxx_std_17)

include(cmake/doctest.cmake)
doctest_discover_tests(simple_telemetry_tests TEST_SPEC *)
target_compile_features(simple_telemetry_tests PUBLIC cxx_std_17)
