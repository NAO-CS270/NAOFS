add_library(inode STATIC "iNode.c" "inCoreiNode.c")

target_include_directories(inode PUBLIC ${PROJECT_INCLUDE_DIRECTORY})
