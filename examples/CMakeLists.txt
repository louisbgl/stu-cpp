set(EXAMPLES duration instant)

foreach(EXAMPLE ${EXAMPLES})
    set(TARGET example_${EXAMPLE})

    add_executable(${TARGET} ${EXAMPLE}.cpp)
    target_link_libraries(${TARGET} PRIVATE stu)

    add_custom_target(run_${EXAMPLE}
        COMMAND ${TARGET}
        DEPENDS ${TARGET}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/examples
        COMMENT "Running example: ${EXAMPLE}"
    )
endforeach()
