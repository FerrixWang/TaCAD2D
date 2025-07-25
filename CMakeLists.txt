cmake_minimum_required(VERSION 3.15)

project(TaCAD VERSION 0.1 LANGUAGES CXX)

# Enable Qt's auto-generation features for UI, MOC, and RCC files
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set C++17 as the standard and enforce its use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Append custom module path for additional CMake scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Configure paths for dependency libraries
set(QT6_DIR "" CACHE PATH "Path to Qt6 installation directory")
set(OpenCASCADE_DIR "" CACHE PATH "Path to OpenCASCADE installation directory")
set(BOOST_ROOT "" CACHE PATH "Path to Boost root directory")

# Add user-specified paths to the CMake prefix path for finding dependencies
if(QT6_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${QT6_DIR})
endif()

if(OpenCASCADE_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${OpenCASCADE_DIR})
endif()

if(BOOST_ROOT)
    list(APPEND CMAKE_PREFIX_PATH ${BOOST_ROOT})
endif()

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets LinguistTools Test OpenGLWidgets)

# Output the location where Qt6 was found
message(STATUS "Found Qt6 at ${Qt6_DIR}")

# Set up standard Qt project configuration
qt_standard_project_setup()

# Include configuration files for dependencies
include(SetupOpenCASCADE)
include(SetupBoost)
include(SetupOpenGL)

# Add subdirectories for building
add_subdirectory(cmake)
add_subdirectory(ext)
add_subdirectory(src)
