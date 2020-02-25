FILE(GLOB_RECURSE SOURCES *.c)

add_executable(
    unit_tests
    ${SOURCES}
        runner.c
)

#find_package(FUSE REQUIRED)
target_link_libraries(
    unit_tests
    gtest_main
    NAOFS
#    ${FUSE_LIBRARIES}
#    -lulockmgr
)

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
  )
