cmake_minimum_required(VERSION 3.8.2)
project(simple_instruments
        VERSION 0.0.0
        LANGUAGES C CXX)

include(cmake/buildsetup.cmake)

option(BUILD_SIMPLE_INSTRUMENTS_TESTS "Set this to ON to build unit tests" ON)

add_library(simple_instruments include/simple_instruments.h)
set_target_properties(simple_instruments PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(simple_instruments INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_include_directories(simple_instruments PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(simple_instruments PROPERTIES PUBLIC_HEADER "include/simple_instruments.h")

install(TARGETS simple_instruments
        EXPORT SimpleInstrumentsTargets
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

include(CMakePackageConfigHelpers)

install(EXPORT SimpleInstrumentsTargets
        NAMESPACE SimpelInstruments::
        DESTINATION lib/cmake/simple_instruments)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/simple_instruments/SimpleInstrumentsConfigVersion.cmake"
        VERSION ${Upstream_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(
        FILES
        cmake/SimpleInstrumentsConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/simple_instruments/SimpleInstrumentsConfigVersion.cmake"
        DESTINATION
        lib/cmake/simple_instruments
        COMPONENT
        Devel
)

if (BUILD_SIMPLE_INSTRUMENTS_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
