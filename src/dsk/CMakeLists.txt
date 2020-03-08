add_library(diskAccess STATIC "blkfetch.c" "diskAccess.c" "mdisk.c")

target_include_directories(diskAccess PUBLIC ${PROJECT_INCLUDE_DIRECTORY})

