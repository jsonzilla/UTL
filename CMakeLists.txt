cmake_minimum_required(VERSION 3.9.1)

project(
    UTL
    VERSION 2.0.0
    DESCRIPTION "Collection of self-contained utility libraries."
    HOMEPAGE_URL "https://github.com/DmitriBogdanov/UTL"
)

# Source
include_directories(include)

# Define UTL as a header-only library
add_library(UTL INTERFACE)

# Specify the include directories for the UTL library using generator expressions
target_include_directories(UTL INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add the UTL target to an export set
install(TARGETS UTL EXPORT UTLConfig DESTINATION lib)

# Create a CMake package configuration file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/UTLConfigVersion.cmake"
    VERSION "2.0.0"
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT UTLConfig
    FILE UTLConfig.cmake
    DESTINATION lib/cmake/UTL
)

# Allow this project to be included as a subdirectory
export(EXPORT UTLConfig FILE "${CMAKE_CURRENT_BINARY_DIR}/UTLConfig.cmake")

# Tests & Benchmarks
#add_subdirectory(tests)
#add_subdirectory(benchmarks)