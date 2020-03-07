add_library(mkfs STATIC "mkfs.c" "iNodeManager.c" "freeBlockList.c" "ialloc.c" "ifree.c" "metaBlocks.c")

target_include_directories(mkfs PUBLIC ${PROJECT_INCLUDE_DIRECTORY})

target_link_libraries(mkfs PUBLIC diskAccess mandsk)
