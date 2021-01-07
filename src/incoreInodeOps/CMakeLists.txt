add_library(incoreInodeOps STATIC bmap.c freeList.c hashQ.c iNodeManager.c iget.c iput.c)

target_include_directories(incoreInodeOps PUBLIC ${PROJECT_INCLUDE_DIRECTORY})
target_link_libraries(incoreInodeOps PUBLIC diskAccess inode diskStruct)
