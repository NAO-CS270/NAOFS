FILE(GLOB SOURCES *.c)

add_executable(
    unit_tests
    ${SOURCES}
    main.c
)

target_link_libraries(
    unit_tests
    gtest_main
    NAOFS
)

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
  )
