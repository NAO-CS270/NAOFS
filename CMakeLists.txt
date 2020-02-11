# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Project's name
project(naofs)

include(FindPkgConfig)

# Headers are in `include` directory
include_directories(include)

pkg_check_modules(FUSE REQUIRED fuse)

file(GLOB SOURCES "src/*.c")

add_executable(naofs ${SOURCES})

target_compile_options(naofs PRIVATE -D_FILE_OFFSET_BITS=64)















#project(gtest-demo CXX)

# place binaries and libraries according to GNU standards
#include(GNUInstallDirs)

#Set build folder
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
# we use this to get code coverage
#if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
#endif()

#add_subdirectory(src)

#include(cmake/googletest.cmake)
#fetch_googletest(
#    ${PROJECT_SOURCE_DIR}/cmake
#    ${PROJECT_BINARY_DIR}/googletest
#    )

#enable_testing()
#add_subdirectory(test)
