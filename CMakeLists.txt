# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(NAOFS CXX)

include(FindPkgConfig)

# place binaries and libraries according to GNU standards
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# Headers are in `include` directory
include_directories(include)
pkg_check_modules(FUSE REQUIRED fuse)
# find_package(FUSE REQUIRED)

#set(CMAKE_C_FLAGS "`pkg-config fuse --cflags --libs`")
set(CMAKE_CXX_FLAGS "-D_FILE_OFFSET_BITS=64 -lulockmgr")
# we use this to get code coverage
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(naofs ${SOURCES})

add_definitions(-rdynamic -g -ggdb)
target_compile_options(naofs PRIVATE -D_FILE_OFFSET_BITS=64 -rdynamic -g -ggdb)

target_include_directories(naofs PRIVATE ${FUSE_INCLUDE_DIR})
target_link_libraries(naofs ${FUSE_LIBRARIES})

include(cmake/googletest.cmake)
fetch_googletest(
    ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_BINARY_DIR}/googletest
    )

enable_testing()
add_subdirectory(test)
