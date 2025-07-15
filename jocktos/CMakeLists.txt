cmake_minimum_required(VERSION 3.21)

# definitions
project(jocktos
        VERSION 1.0.0
        DESCRIPTION "This is JOCKTOS"
)

enable_language(C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED  ON)

set(CMAKE_C_EXTENSIONS OFF)

set(MCU_OPTIONS
    -mcpu=cortex-m4
    -mthumb
)

set (EXTRA_OPTIONS
    -fdata-sections
    -ffunction-sections
)

set (OPTIMIZATION_OPTIONS
$<$<CONFIG:Debug>:-O0>
$<$<CONFIG:Release>:-O3>
)

set(DEPENDENCY_INFO_OPTIONS
    -MMD
    -MP
)

set(DEBUG_INFO_OPTIONS
    -g3
    -gdwarf-2
)

add_compile_options(
    ${MCU_OPTIONS}
    ${EXTRA_OPTIONS}
    ${DEBUG_INFO_OPTIONS}
    ${DEPENDENCY_INFO_OPTIONS}
    ${OPTIMIZATION_OPTIONS}
)

add_link_options(
    ${MCU_OPTIONS}
    -specs=nano.specs
    -T${CMAKE_SOURCE_DIR}/CubeMX/STM32F303RETx_FLASH.ld
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)

link_libraries("-lc -lm -lnosys")

set(EXECUTABLE ${PROJECT_NAME}.elf)

# list of modules to build final firmware
add_executable(${EXECUTABLE}
    src/main.c
    src/allocator.c
    src/comm.c
    src/io_list.c 
    src/io.c 
    src/os.c
    src/synchro.c
    src/sys.c
    src/tcb.c
    src/timers.c
    CubeMX/system_stm32f3xx.c
    CubeMX/startup_stm32f303xe.s
    )

target_compile_definitions(${EXECUTABLE}
PRIVATE 
    STM32F303xE
)
target_compile_options(${EXECUTABLE}
PRIVATE
    -Wall
    -Wextra
    -Wshadow
    -Wconversion
    -Wdouble-promotion
)

target_include_directories(${EXECUTABLE}
PRIVATE
    inc
    CubeMX

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
