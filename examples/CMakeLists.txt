add_executable(HelloWindow WIN32)

target_sources(HelloWindow PRIVATE
    hello_window.cpp
)
target_link_libraries(HelloWindow PRIVATE
    RigelLib::RigelLib
)

rigel_enable_warnings(HelloWindow)
