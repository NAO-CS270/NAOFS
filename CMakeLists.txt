cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(NAOFS C)

include(FindPkgConfig)

pkg_check_modules(FUSE REQUIRED fuse)

#set(CMAKE_C_FLAGS "-pg -D_FILE_OFFSET_BITS=64 -lulockmgr")
#set(CMAKE_C_FLAGS "-mcmodel=large")

pkg_check_modules(FUSE REQUIRED fuse)
# find_package(FUSE REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.c")
list(REMOVE_ITEM SOURCES "main.c")
add_library (NAOfs ${SOURCES})
add_executable(naofs ${SOURCES} "src/main.c")

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
target_link_libraries(naofs PUBLIC fuse diskAccess incoreInodeOps inode interface mandsk diskStruct trav utils)

#target_compile_options(naofs PUBLIC -D_FILE_OFFSET_BITS=64)
#add_library (NAOfs ${SOURCES})
#add_definitions(-rdynamic -g -ggdb -pg)
#target_compile_options(naofs PRIVATE -D_FILE_OFFSET_BITS=64 -rdynamic -g -ggdb -pg)

include(CTest)
enable_testing()
add_executable(naoTest "test/runner.c" "test/utils/test_utils.c")
target_link_libraries(naoTest PUBLIC fuse diskAccess incoreInodeOps inode interface mandsk diskStruct trav utils)
add_test (NAME NAOfsTest COMMAND naoTest)
