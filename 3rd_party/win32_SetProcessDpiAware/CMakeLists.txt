set(sources
    win32_SetProcessDpiAware.h
    win32_SetProcessDpiAware.c
)

add_library(win32_SetProcessDpiAware STATIC ${sources})
target_include_directories(win32_SetProcessDpiAware
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
