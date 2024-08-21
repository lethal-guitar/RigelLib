# Header-only targets
###############################################################################

add_library(glm INTERFACE)
target_include_directories(glm
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/glm"
)
add_library(nlohmann-json INTERFACE)
target_include_directories(nlohmann-json
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann"
)
add_library(static_vector INTERFACE)
target_include_directories(static_vector
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/static_vector/include"
)
add_library(lyra INTERFACE)
target_include_directories(lyra
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/lyra/include"
)


# Compiled targets
###############################################################################

# EXCLUDE_FROM_ALL to disable installing these HO or static library deps
add_subdirectory(glad EXCLUDE_FROM_ALL)
add_subdirectory(imgui EXCLUDE_FROM_ALL)
add_subdirectory(loguru EXCLUDE_FROM_ALL)
add_subdirectory(speex_resampler EXCLUDE_FROM_ALL)
add_subdirectory(stb EXCLUDE_FROM_ALL)

if (WIN32)
    add_subdirectory(win32_SetProcessDpiAware EXCLUDE_FROM_ALL)
endif()
