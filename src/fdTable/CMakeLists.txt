add_library(fdTable STATIC fileTables.c globalFileTable.c)

target_include_directories(fdTable PUBLIC ${PROJECT_INCLUDE_DIRECTORY})
target_link_libraries(fdTable PRIVATE utils)
