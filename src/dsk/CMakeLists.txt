add_library(diskAccess STATIC "blkfetch.c" "mdisk.c" "alloc.c" "free.c")

target_include_directories(diskAccess PUBLIC ${PROJECT_INCLUDE_DIRECTORY})

