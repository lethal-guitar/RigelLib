include(${CMAKE_CURRENT_LIST_DIR}/deps.cmake)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR})

rigel_set_global_compiler_flags()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
