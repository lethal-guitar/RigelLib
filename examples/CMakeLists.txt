add_executable(Bootstrap WIN32)

target_sources(Bootstrap PRIVATE
    bootstrap.cpp
)
target_link_libraries(Bootstrap PRIVATE
    RigelLib::RigelLib
)

rigel_enable_warnings(Bootstrap)
