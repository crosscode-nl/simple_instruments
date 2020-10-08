cmake_minimum_required(VERSION 3.8.2)
project(simple_telemetry
        VERSION 0.0.0
        LANGUAGES C CXX)

include(cmake/buildsetup.cmake)

option(BUILD_SIMPLE_TELEMETRY_TESTS "Set this to ON to build unit tests" ON)

set(SIMPLE_TELEMETRY_HEADERS
    include/simple_telemetry.h
        include/simple_telemetry/telemetry.h)

set(SIMPLE_TELEMETRY_SRC src/simple_telemetry/telemetry.cpp)

add_library(simple_telemetry ${SIMPLE_TELEMETRY_HEADERS} ${SIMPLE_TELEMETRY_SRC})
target_include_directories(simple_telemetry INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_include_directories(simple_telemetry PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(simple_telemetry PROPERTIES PUBLIC_HEADER "include/simple_telemetry.h")

if (BUILD_SIMPLE_TELEMETRY_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
