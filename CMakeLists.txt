# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.7)

# Project's name
project(SDLPong)

# Adding finding modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find SDL
find_package(SDL2 REQUIRED)

# SDL extensions
find_package(SDL2_image REQUIRED)
find_package(SDL2_NET REQUIRED)

# Directories to be included
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_NET_INCLUDE_DIRS} ${SDL2_IMG_INCLUDE_DIRS})

# Add src directory
add_subdirectory(src)

