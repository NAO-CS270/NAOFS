add_library(NAOFS "")

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

#FILE(GLOB_RECURSE SOURCES ./**.c)
#list(FILTER SOURCES EXCLUDE REGEX ".*main.c$")
#list(REMOVE_ITEM SOURCES "main.c")
set(SOURCES utils/utils.c)

#FILE(GLOB_RECURSE HEADERS ./**.h)
#list(FILTER HEADERS EXCLUDE REGEX ".*main.h$")
set(HEADERS utils/utils.h)

#print_all_variables()
#find_package(FUSE REQUIRED)
#target_link_libraries(homefs ${FUSE_LIBRARIES})
target_sources(
    NAOFS
    PRIVATE
        ${SOURCES}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/${HEADERS}
    )

target_include_directories(
    NAOFS
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
#    PRIVATE
#        ${FUSE_INCLUDE_DIR}
    )
