set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

find_package(SDL2 REQUIRED)
find_package(Filesystem REQUIRED COMPONENTS Final)
find_package(Boost)
find_package(Git)
