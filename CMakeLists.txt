cmake_minimum_required(VERSION 3.24)

project(quickstats
    VERSION 0.1.0
    DESCRIPTION "Quickly compute simple statistics"
    LANGUAGES CXX)

add_library(quickstats INTERFACE)
add_library(ltla::quickstats ALIAS quickstats)

target_compile_features(quickstats INTERFACE cxx_std_17)

option(QUICKSTATS_FETCH_EXTERN "Automatically fetch quickstats's external dependencies." ON)
if(QUICKSTATS_FETCH_EXTERN)
    add_subdirectory(extern)
else()
    find_package(ltla_sanisizer 0.2.0 CONFIG REQUIRED)
endif()

target_link_libraries(quickstats INTERFACE ltla::sanisizer)

include(GNUInstallDirs)
target_include_directories(quickstats INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ltla_quickstats>"
)

# Building the test-related machinery, if we are compiling this library directly.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(QUICKSTATS_TESTS "Build quickstats's test suite." ON)
else()
    option(QUICKSTATS_TESTS "Build quickstats's test suite." OFF)
endif()

if(QUICKSTATS_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

# Installing for find_package.
include(CMakePackageConfigHelpers)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ltla_quickstats)

install(TARGETS quickstats
    EXPORT quickstatsTargets)

install(EXPORT quickstatsTargets
    FILE ltla_quickstatsTargets.cmake
    NAMESPACE ltla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_quickstats)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_quickstatsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_quickstats)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ltla_quickstatsConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ltla_quickstatsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_quickstatsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_quickstats)
