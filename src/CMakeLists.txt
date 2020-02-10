add_library(NAOFS "")

FILE(GLOB SOURCES *.c)

FILE(GLOB HEADERS *.h)

find_package(FUSE REQUIRED)
target_link_libraries(homefs ${FUSE_LIBRARIES})
target_sources(
    NAOFS
    PRIVATE
        ${SOURCES}
    PUBLIC
        ${HEADERS}
    )

target_include_directories(
    NAOFS
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
        ${FUSE_INCLUDE_DIR}
    )
