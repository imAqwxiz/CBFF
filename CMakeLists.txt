
cmake_minimum_required(VERSION 3.21)

project(CBFF VERSION 1.0.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Architecture
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

# Visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# Create mod library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Locate Geode SDK
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR
        "Unable to find Geode SDK! "
        "Please define GEODE_SDK environment variable."
    )
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

# Include Geode SDK
add_subdirectory(
    $ENV{GEODE_SDK}
    ${CMAKE_CURRENT_BINARY_DIR}/geode
)

# Configure Geode mod
setup_geode_mod(${PROJECT_NAME})

