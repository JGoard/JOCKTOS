# Define the board support library
add_library(board_STM32F303xE
    startup_stm32f303xe.s
    system_stm32f3xx.c
    system_stm32f3xx.h
)

target_include_directories(board_STM32F303xE PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/cmsis
)

# Linker script for this board
# target_link_options(board_STM32F303xE INTERFACE
#     -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F303RETx_FLASH.ld
# )

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F303RETx_FLASH.ld)
set_target_properties(board_STM32F303xE PROPERTIES
    INTERFACE_LINK_DEPENDS "${LINKER_SCRIPT}"
    INTERFACE_LINK_OPTIONS "-T${LINKER_SCRIPT}"
)