cmake_minimum_required(VERSION 3.8.2)
project(simple_instruments_tests LANGUAGES C CXX)

list(APPEND TEST_SRC
        main.cpp
        simple_instruments_tests.cpp
)

add_executable(simple_instruments_tests ${TEST_SRC})
target_link_libraries(simple_instruments_tests simple_instruments)
target_include_directories(simple_instruments_tests PUBLIC include)
target_compile_features(simple_instruments_tests PUBLIC cxx_std_17)

include(cmake/doctest.cmake)
doctest_discover_tests(simple_instruments_tests TEST_SPEC *)
target_compile_features(simple_instruments_tests PUBLIC cxx_std_17)
