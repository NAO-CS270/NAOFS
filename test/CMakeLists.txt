FILE(GLOB SOURCES *.c)

add_executable(
    unit_tests
    ${SOURCES}
    main.c
)

find_package(FUSE REQUIRED)
target_link_libraries(
    unit_tests
    gtest_main
    NAOFS
    ${FUSE_LIBRARIES}
)

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
  )
