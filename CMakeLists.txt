# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(NAOFS C)

include(FindPkgConfig)

# Headers are in `include` directory
include_directories(include)
pkg_check_modules(FUSE REQUIRED fuse)

set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -lulockmgr")

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(naofs ${SOURCES})

add_definitions(-rdynamic -g -ggdb)
target_compile_options(naofs PRIVATE -D_FILE_OFFSET_BITS=64 -rdynamic -g -ggdb)

target_include_directories(naofs PRIVATE ${FUSE_INCLUDE_DIR})
target_link_libraries(naofs ${FUSE_LIBRARIES})

#include(cmake/googletest.cmake)
#fetch_googletest(
#    ${PROJECT_SOURCE_DIR}/cmake
#    ${PROJECT_BINARY_DIR}/googletest
#    )

enable_testing()
#add_subdirectory(test)
