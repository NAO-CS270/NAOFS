add_library(NAOFS "")

execute_process (
    COMMAND bash -c "find . -regex '.*.c$'"
    SOURCES outVar
)

execute_process (
        COMMAND bash -c "find . -regex '.*.c$'"
        HEADERS outVar
)

target_sources(
    NAOFS
    PRIVATE
        example.cpp
        ${SOURCES}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/example.h
        ${HEADERS}
    )

target_include_directories(
    NAOFS
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )
