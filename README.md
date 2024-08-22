# RigelLib

This library offers a collection of reusable code and utilities for C++ applications based on SDL and OpenGL. Most of this was originally written for [RigelEngine](https://github.com/lethal-guitar/RigelEngine).


# How to use this

The intended way of integrating RigelLib into a project is to add the code as a git submodule (or copy it), and then use CMake's `add_subdirectory` function to include it in the build. More detailed instructions coming soon.

Also see the [examples](/examples).

# What's in here?

The library is organized into a few different modules:

### `base`

This is essentially an "extended standard library" containing various utilities. It includes types for working with 2D bitmaps (`Image`, `Color`), functions for loading of common image formats like `.png` and `.jpeg`, some 2D vector math, helpers for binary file format I/O, and useful C++ utilities like `match` (more convenient `std::variant` visitation) and `ArrayView` (similar to `std::span`).

## `opengl`

OpenGL header and a shader class.

## `sdl_utils`

Mostly tools to make working with SDL easier in C++, specifically memory management and error handling.

## `ui`

Dear ImGui integration and an FPS counter.


