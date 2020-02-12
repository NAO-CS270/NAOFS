# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Project's name
project(naofs)

include(FindPkgConfig)

# Headers are in `include` directory
include_directories(include)

pkg_check_modules(FUSE REQUIRED fuse)
# find_package(FUSE REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(naofs ${SOURCES})

add_definitions(-rdynamic -g -ggdb)
target_compile_options(naofs PRIVATE -D_FILE_OFFSET_BITS=64 -rdynamic -g -ggdb)

target_include_directories(naofs PRIVATE ${FUSE_INCLUDE_DIR})
target_link_libraries(naofs ${FUSE_LIBRARIES})
