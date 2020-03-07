add_library(trav STATIC "directory.c" "namei.c")

target_include_directories(trav PUBLIC ${PROJECT_INCLUDE_DIRECTORY})

target_link_libraries(trav PUBLIC diskAccess incoreInodeOps)
