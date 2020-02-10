add_library(NAOFS "")

FILE(GLOB SOURCES *.c)

FILE(GLOB HEADERS *.h)

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
    )
