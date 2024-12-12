cmake_minimum_required(VERSION 3.1)

# definitions
project(jocktos)
set(BOARD "-DSTM32F303xE")
# select linker script
set(LINKER_SCRIPT "CubeMX/STM32F303RETx_FLASH.ld")

#define CPU Options
set(CPU_OPTIONS -mcpu=cortex-m4 -mthumb) # Insert Floating point unit options here whenever it decides to start working

# optimizations and flags
set(CMAKE_C_FLAGS_DEBUG     "-g -O0" )
set(CMAKE_C_FLAGS_RELEASE   "-Os")

add_compile_options(
    ${CPU_OPTIONS}
    $<$<COMPILE_LANGUAGE:C>:-std=c99>
    -fstrict-volatile-bitfields
    -ffunction-sections
    -Wall 
    -Wextra 
    -Wnull-dereference 
    -Wno-override-init 
    -fdata-sections 
    -ffunction-sections 
    -fno-exceptions
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    inc/
    CubeMx/
)

link_directories(
    ${CMAKE_SOURCE_DIR}
)

# list of modules to build final firmware
add_executable(${PROJECT_NAME}
    src/main.c
    src/allocator.c
    src/comm.c
    src/io_list.c 
    src/os.c
    src/synchro.c
    src/sys.c
    src/tcb.c
    src/timers.c
    CubeMX/system_stm32f3xx.c
    )

target_link_libraries(${PROJECT_NAME}
    ${CPU_OPTIONS}
    -T${LINKER_SCRIPT}
    -nostartfiles
    m
    # -nostdlib
)

set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_DEPENDS ${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT})

include("cmake/flash.cmake")