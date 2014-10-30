cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CmakeModules/")
project(echocounter)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
