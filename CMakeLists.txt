# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.7)

# Project's name
project(ArchersDuel)

# C++ version flag

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

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

