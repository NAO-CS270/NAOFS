add_library(diskStruct STATIC "mkfs.c" "iNodeManager.c" "freeBlockList.c" "ialloc.c" "ifree.c" "metaBlocks.c")

target_include_directories(diskStruct PUBLIC ${PROJECT_INCLUDE_DIRECTORY})

target_link_libraries(diskStruct PUBLIC diskAccess mandsk)

add_executable(mkfs "exec-mkfs.c")
target_include_directories(mkfs PUBLIC ${PROJECT_INCLUDE_DIRECTORY})
target_link_libraries(mkfs PUBLIC diskStruct inode)

add_executable(diskViewer "diskReader.c")
target_include_directories(diskViewer PUBLIC ${PROJECT_INCLUDE_DIRECTORY})
target_link_libraries(diskViewer PUBLIC diskStruct inode)
