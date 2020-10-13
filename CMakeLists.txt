cmake_minimum_required(VERSION 3.8.2)
project(simple_instruments
        VERSION 0.3.0
        LANGUAGES C CXX)

include(cmake/buildsetup.cmake)

option(BUILD_SIMPLE_INSTRUMENTS_TESTS "Set this to ON to build unit tests" ON)

add_library(simple_instruments INTERFACE)
target_compile_features(simple_instruments INTERFACE cxx_std_17)

target_include_directories(simple_instruments INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

install(TARGETS simple_instruments
        EXPORT SimpleInstrumentsTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/SimpleInstruments/SimpleInstrumentsConfigVersion.cmake"
        VERSION ${Upstream_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(EXPORT SimpleInstrumentsTargets
        NAMESPACE SimpleInstruments::
        DESTINATION lib/cmake/SimpleInstruments)

install(
        FILES
        cmake/SimpleInstrumentsConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/SimpleInstruments/SimpleInstrumentsConfigVersion.cmake"
        DESTINATION
        lib/cmake/SimpleInstruments
        COMPONENT
        Devel
)

install(FILES ${PROJECT_SOURCE_DIR}/include/simple_instruments.h DESTINATION include)

if (BUILD_SIMPLE_INSTRUMENTS_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
