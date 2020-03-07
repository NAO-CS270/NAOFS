add_library(interface STATIC "close.c" "create.c" "getAttr.c" "link.c" "open.c" "read.c" "truncate.c" "unlink.c" "write.c")

target_include_directories(interface PUBLIC ${PROJECT_INCLUDE_DIRECTORY})
target_link_libraries(interface PUBLIC fdTable)
target_compile_options(interface PUBLIC -D_FILE_OFFSET_BITS=64)
