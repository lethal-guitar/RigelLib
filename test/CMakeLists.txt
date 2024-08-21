Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)


add_executable(tests
    test_array_view.cpp
    test_rectangle.cpp
    test_string_utils.cpp
)

target_link_libraries(tests
    PRIVATE
    RigelLib
    Catch2::Catch2WithMain
)

rigel_enable_warnings(tests)

# Ignore warnings emitted from Catch macros
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(tests
        PRIVATE
        -Wno-parentheses
    )
endif()

add_test(all-tests tests)
