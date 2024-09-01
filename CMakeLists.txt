cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if (DEFINED PROJECT_NAME)
    set(RIGEL_IS_BUNDLED ON)
else()
    set(RIGEL_IS_BUNDLED OFF)
endif()

project(RigelLib VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Project settings
###############################################################################

option(RIGEL_USE_GL_ES "Use OpenGL ES instead of regular OpenGL" OFF)
option(RIGEL_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(RIGEL_BUILD_TESTS "Build tests" OFF)
option(RIGEL_BUILD_EXAMPLES "Build examples" OFF)

if (NOT RIGEL_IS_BUNDLED)
    if (NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
      message(STATUS "No build type was specified, will default to ${CMAKE_BUILD_TYPE}")
    endif()

    message(STATUS "********************************************************************************")
    message(STATUS "${PROJECT_NAME}, build type: ${CMAKE_BUILD_TYPE}, version: ${PROJECT_VERSION}")
    message(STATUS "********************************************************************************")
else()
    set(RIGEL_BUILD_TESTS OFF)
    set(RIGEL_BUILD_EXAMPLES OFF)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/rigel_sanitizers.cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


# Dependencies
###############################################################################

if (NOT RIGEL_IS_BUNDLED)
    include(deps.cmake)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/rigel.cmake")

# This catches the most common case, where none of the submodules have been
# initialized. It's still possible to get in a state where imgui is present
# but other submodules are not, but that's unlikely to happen to someone who
# has just cloned the repo and wants to build for the first time.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/imgui/imgui/imgui.h")
    message(FATAL_ERROR
        "It seems git submodules were not initialized. You need to run git submodule update --init --recursive, and then re-run CMake.")
endif()


# Build targets
###############################################################################

rigel_set_global_compiler_flags()


add_subdirectory(3rd_party)
add_subdirectory(src)

if(RIGEL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(RIGEL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


add_library(RigelLib::RigelLib ALIAS RigelLib)
add_library(RigelLib::lyra ALIAS lyra)
add_library(RigelLib::SDL2 ALIAS SDL2::Core)
