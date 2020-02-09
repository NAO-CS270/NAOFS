execute_process (
        COMMAND bash -c "find . -regex '.*.c$'"
        SOURCES outVar
)

add_executable(
    unit_tests
    ${SOURCES}
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
