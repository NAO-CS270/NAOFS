# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(NAOFS C)

include(FindPkgConfig)

# Headers are in `include` directory
include_directories(include)
pkg_check_modules(FUSE REQUIRED fuse)

set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -lulockmgr")

file(GLOB_RECURSE SOURCES "src/*.c")
list(REMOVE_ITEM SOURCES "main.c")
add_library (NAOfs ${SOURCES})
add_executable(naofs ${SOURCES} "src/main.c")

add_definitions(-rdynamic -g -ggdb)
target_compile_options(naofs PRIVATE -D_FILE_OFFSET_BITS=64 -rdynamic -g -ggdb)

target_include_directories(naofs PRIVATE ${FUSE_INCLUDE_DIR})
target_link_libraries(naofs ${FUSE_LIBRARIES})

#include(CTest)
enable_testing()
#
file(GLOB_RECURSE TEST_SOURCES "test/*.c")
list(REMOVE_ITEM TEST_SOURCES "test/test_block_fetch.c")
add_executable (Test ${TEST_SOURCES})
target_link_libraries(Test NAOfs)
#
add_test (NAME NAOfsTest COMMAND Test)
