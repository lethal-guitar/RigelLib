set(sources
    ../include/rigel/base/array_view.hpp
    ../include/rigel/base/binary_io.hpp
    ../include/rigel/base/byte_buffer.hpp
    ../include/rigel/base/clock.hpp
    ../include/rigel/base/container_utils.hpp
    ../include/rigel/base/defer.hpp
    ../include/rigel/base/grid.hpp
    ../include/rigel/base/image.hpp
    ../include/rigel/base/image_loading.hpp
    ../include/rigel/base/math_utils.hpp
    ../include/rigel/base/spatial_types.hpp
    ../include/rigel/base/static_vector.hpp
    ../include/rigel/base/string_utils.hpp
    ../include/rigel/base/warnings.hpp
    ../include/rigel/opengl/opengl.hpp
    ../include/rigel/opengl/shader.hpp
    ../include/rigel/sdl_utils/error.hpp
    ../include/rigel/sdl_utils/key_code.hpp
    ../include/rigel/sdl_utils/platform.hpp
    ../include/rigel/sdl_utils/ptr.hpp
    ../include/rigel/ui/fps_display.hpp
    ../include/rigel/ui/imgui_integration.hpp
    ../include/rigel/bootstrap.hpp

    base/array_view.cpp
    base/byte_buffer.cpp
    base/image.cpp
    base/image_loading.cpp
    base/string_utils.cpp
    opengl/opengl.cpp
    opengl/shader.cpp
    sdl_utils/error.cpp
    sdl_utils/platform.cpp
    ui/fps_display.cpp
    ui/imgui_integration.cpp
    bootstrap.cpp
)


add_library(RigelLib STATIC ${sources})

target_include_directories(RigelLib
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"

    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/rigel"
)
target_link_libraries(RigelLib
    PUBLIC
    SDL2::Core
    dear_imgui
    glad
    glm
    loguru
    lyra
    nlohmann-json
    static_vector
    std::filesystem

    PRIVATE
    speex_resampler
    stb
)

if (TARGET Boost::boost)
    target_link_libraries(RigelLib PUBLIC Boost::boost)
endif()

if (WIN32)
    target_link_libraries(RigelLib PUBLIC win32_SetProcessDpiAware)
endif()

rigel_enable_warnings(RigelLib)

if(RIGEL_USE_GL_ES)
    target_compile_definitions(RigelLib PUBLIC
        RIGEL_USE_GL_ES=1
    )
endif()
