set(EXECUTABLE default.elf)

add_executable(${EXECUTABLE} 
    main.c
)

target_link_libraries(${EXECUTABLE} PRIVATE 
    jocktos
    board_${BOARD}
)

target_sources(${EXECUTABLE} PRIVATE
    $<TARGET_OBJECTS:jocktos>
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/jocktos
)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE}
)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
)
