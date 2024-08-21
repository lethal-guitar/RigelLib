set(sources
    ../include/rigel/base/array_view.hpp
    ../include/rigel/base/byte_buffer.hpp
    ../include/rigel/base/clock.hpp
    ../include/rigel/base/container_utils.hpp
    ../include/rigel/base/defer.hpp
    ../include/rigel/base/file_utils.hpp
    ../include/rigel/base/grid.hpp
    ../include/rigel/base/image.hpp
    ../include/rigel/base/image_loading.hpp
    ../include/rigel/base/math_utils.hpp
    ../include/rigel/base/spatial_types.hpp
    ../include/rigel/base/static_vector.hpp
    ../include/rigel/base/string_utils.hpp
    ../include/rigel/base/warnings.hpp
    ../include/rigel/sdl_utils/error.hpp
    ../include/rigel/sdl_utils/key_code.hpp
    ../include/rigel/sdl_utils/platform.hpp
    ../include/rigel/sdl_utils/ptr.hpp
    ../include/rigel/ui/fps_display.hpp
    ../include/rigel/ui/imgui_integration.hpp

    base/array_view.cpp
    base/file_utils.cpp
    base/image.cpp
    base/image_loading.cpp
    base/string_utils.cpp
    sdl_utils/error.cpp
    sdl_utils/platform.cpp
    ui/fps_display.cpp
    ui/imgui_integration.cpp
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

if(RIGEL_USE_GL_ES)
    target_compile_definitions(RigelLib PUBLIC
        RIGEL_USE_GL_ES=1
    )
endif()
