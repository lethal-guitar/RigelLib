cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(RigelLib VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Project settings
###############################################################################

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  message(STATUS "No build type was specified, will default to ${CMAKE_BUILD_TYPE}")
endif()

option(USE_GL_ES "Use OpenGL ES instead of regular OpenGL" OFF)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(BUILD_TESTS "Build tests" OFF)

message(STATUS "********************************************************************************")
message(STATUS "${PROJECT_NAME}, build type: ${CMAKE_BUILD_TYPE}, version: ${PROJECT_VERSION}")
message(STATUS "********************************************************************************")

include("${CMAKE_SOURCE_DIR}/cmake/rigel_sanitizers.cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Dependencies
###############################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include("${CMAKE_SOURCE_DIR}/cmake/rigel.cmake")

# This catches the most common case, where none of the submodules have been
# initialized. It's still possible to get in a state where imgui is present
# but other submodules are not, but that's unlikely to happen to someone who
# has just cloned the repo and wants to build for the first time.
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/3rd_party/imgui/imgui/imgui.h")
    message(FATAL_ERROR
        "It seems git submodules were not initialized. You need to run git submodule update --init --recursive, and then re-run CMake.")
endif()

find_package(SDL2 REQUIRED)
find_package(Filesystem REQUIRED COMPONENTS Final)
find_package(Boost)
find_package(Git)

if (TARGET Boost::boost)
    add_definitions(-DRIGEL_HAVE_BOOST)
endif()


# Compiler settings
###############################################################################

if(MSVC)
    add_compile_options(
        /Zc:__cplusplus
        /permissive-
        /MP
        /EHsc
        /utf-8
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -fcolor-diagnostics
    )
endif()


# Build targets
###############################################################################

add_subdirectory(3rd_party)
