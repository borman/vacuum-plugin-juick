cmake_minimum_required(VERSION 2.8)

project(juick-plugin)
set(PLUGIN_NAME juick)

set(SOURCES juickplugin.cpp)
set(HEADERS juickplugin.h)
set(MOC_HEADERS juickplugin.h)

include_directories("${CMAKE_SOURCE_DIR}")

include("${CMAKE_SOURCE_DIR}/cmake/config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/plugin.cmake")
