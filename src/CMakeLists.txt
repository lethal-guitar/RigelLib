set(sources
    base/array_view.cpp
    base/array_view.hpp
    base/byte_buffer.hpp
    base/clock.hpp
    base/container_utils.hpp
    base/defer.hpp
    base/file_utils.cpp
    base/file_utils.hpp
    base/grid.hpp
    base/image.cpp
    base/image.hpp
    base/image_loading.cpp
    base/image_loading.hpp
    base/math_utils.hpp
    base/spatial_types.hpp
    base/static_vector.hpp
    base/string_utils.cpp
    base/string_utils.hpp
    base/warnings.hpp
    sdl_utils/error.cpp
    sdl_utils/error.hpp
    sdl_utils/key_code.hpp
    sdl_utils/platform.cpp
    sdl_utils/platform.hpp
    sdl_utils/ptr.hpp
    ui/fps_display.cpp
    ui/fps_display.hpp
    ui/imgui_integration.cpp
    ui/imgui_integration.hpp
)


add_library(RigelLib STATIC ${sources})

target_include_directories(RigelLib
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(RigelLib
    PUBLIC
    dear_imgui
    SDL2::Core
    glad
    glm
    loguru
    nlohmann-json
    static_vector
    std::filesystem

    PRIVATE
    stb
    speex_resampler
)

if (TARGET Boost::boost)
    target_link_libraries(RigelLib PUBLIC Boost::boost)
endif()

if (WIN32)
    target_link_libraries(RigelLib PUBLIC win32_SetProcessDpiAware)
endif()

rigel_enable_warnings(RigelLib)

if(USE_GL_ES)
    target_compile_definitions(RigelLib PUBLIC
        RIGEL_USE_GL_ES=1
    )
endif()
