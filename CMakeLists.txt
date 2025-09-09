cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(JockTOS VERSION 1.0.0 LANGUAGES C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(EXAMPLE default CACHE STRING "Example folder in examples/")
set(BOARD STM32F303xE CACHE STRING "Target board folder in boards/")
set(USE_BITMAP_ALLOCATOR ON CACHE BOOL "Use bitmap allocator for buffer allocation")

set(MCU_OPTIONS
    -mcpu=cortex-m4
    -mthumb
)

set(EXTRA_OPTIONS
    -fdata-sections
    -ffunction-sections
    -fomit-frame-pointer
)

set(OPTIMIZATION_OPTIONS
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

set(WARN_OPTIONS
    -Wall
    -Wextra
    -Wshadow
    -Wconversion
    -Wdouble-promotion
)

add_compile_options(
    ${MCU_OPTIONS}
    ${EXTRA_OPTIONS}
    ${DEBUG_INFO_OPTIONS}
    ${DEPENDENCY_INFO_OPTIONS}
    ${OPTIMIZATION_OPTIONS}
    ${WARN_OPTIONS}
)

add_link_options(
    ${MCU_OPTIONS}
    -specs=nano.specs
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)

link_libraries("-lc -lm -lnosys")

# add_compile_definitions(BOARD=${BOARD})
add_compile_definitions(${BOARD})
add_subdirectory(jocktos)
add_subdirectory(examples/${EXAMPLE})
add_subdirectory(boards/${BOARD})