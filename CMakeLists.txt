cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(NAOFS C)

#set(CMAKE_C_FLAGS "-pg -D_FILE_OFFSET_BITS=64 -lulockmgr")
#set(CMAKE_C_FLAGS "-mcmodel=large")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(PkgConfig REQUIRED)
# find_package(fuse3 REQUIRED CONFIG)

pkg_check_modules(FUSE REQUIRED IMPORTED_TARGET fuse3)

set(PROJECT_SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/src")
set(PROJECT_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/include")

add_subdirectory(src/dsk)
add_subdirectory(src/fdTable)
add_subdirectory(src/incoreInodeOps)
add_subdirectory(src/inode)
add_subdirectory(src/interface)
add_subdirectory(src/mandsk)
add_subdirectory(src/mkfs)
add_subdirectory(src/trav)
add_subdirectory(src/utils)

add_executable(naofs "src/main.c")
target_include_directories(naofs PUBLIC ${PROJECT_INCLUDE_DIRECTORY})
target_link_libraries(naofs PUBLIC PkgConfig::FUSE diskAccess incoreInodeOps inode interface mandsk diskStruct trav utils)

