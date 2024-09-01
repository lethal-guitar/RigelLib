add_library(dear_imgui STATIC
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdl2.cpp

    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)
target_include_directories(dear_imgui
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)
target_link_libraries(dear_imgui
    PRIVATE
    SDL2::Core
    stb
)

if (APPLE)
    target_link_libraries(dear_imgui
        PRIVATE
        "-framework AppKit"
    )
endif()


target_compile_definitions(dear_imgui
    PRIVATE
    -DIMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
    "-DIMGUI_STB_RECT_PACK_FILENAME=<stb_rect_pack.h>"
)

if (RIGEL_USE_GL_ES)
    target_compile_definitions(dear_imgui
        PRIVATE
        -DIMGUI_IMPL_OPENGL_ES2
    )
endif()

rigel_disable_warnings(dear_imgui)
