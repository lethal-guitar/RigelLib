# RigelLib

This library offers a collection of reusable code and utilities for C++ applications based on SDL and OpenGL. Most of this is based on code originally written for [RigelEngine](https://github.com/lethal-guitar/RigelEngine).


# How to use this

The easiest way to integrate RigelLib into a project is to add the code as a git submodule, or copy it into your source tree.

Set up the submodule:

```bash
git submodule add https://github.com/lethal-guitar/RigelLib.git RigelLib
git submodule update --init --recursive
```

Then add a `CMakeLists.txt` like this:

```
# 3.13 is the minimum required by RigelLib. You can of course require a higher
# version here
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(MyAwesomeGame)

# This will build the library as part of your project and make its CMake targets
# available (including SDL2::Core and SDL2::Main).
# It also does the necessary find_package() call to locate SDL2.
include(RigelLib/setup.cmake)

# Now define your own build targets, and depend on RigelLib
add_executable(MyAwesomeGame
    main.cpp
)
target_link_libraries(MyAwesomeGame PRIVATE
    SDL2::Main
    RigelLib::RigelLib
)
```

And that's it! You now have the foundation of a SDL2+OpenGL application.

Also see the [examples](/examples).

# What's in here?

The library is organized into a few different modules:

### `bootstrap.hpp` (top-level)

Mini-framework for setting up an SDL+OpenGL application. Provides a helper function which initializes SDL and OpenGL and creates an application window, then repeatedly calls a user-provided "new frame" function.
There's also command line parsing functionality (based on the [Lyra](https://github.com/bfgroup/Lyra) library).
The code handles some Windows-specific intricacies like enabling high DPI support and allowing terminal output when running as a GUI application (subsystem "Windows").

See the [`bootstrap.cpp` example](/examples/bootstrap.cpp).

### `base`

This is essentially an "extended standard library" containing various utilities. It includes types for working with 2D bitmaps (`Image`, `Color`), functions for loading of common image formats like `.png` and `.jpeg`, some 2D vector math, helpers for binary file format I/O, and useful C++ utilities like `match` (more convenient `std::variant` visitation) and `ArrayView` (similar to `std::span`).

### `opengl`

OpenGL header and a shader class.

### `sdl_utils`

Mostly tools to make working with SDL easier in C++, specifically memory management and error handling.

### `ui`

Dear ImGui integration and an FPS counter.


