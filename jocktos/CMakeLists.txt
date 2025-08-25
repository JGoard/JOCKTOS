cmake_minimum_required(VERSION 3.21)
include(FetchContent)

if(NOT TARGET bitmap_allocator)
    message(STATUS "Fetching bitmap_allocator...")
    FetchContent_Declare(
        bitmap_allocator
        GIT_REPOSITORY https://github.com/nick-a-schneider/bitmap_allocator.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(bitmap_allocator)
endif()
if(NOT TARGET bitmap_allocator)
    message(FATAL_ERROR "bitmap_allocator not found")
endif()

# Build the static library
add_library(jocktos 
    core/sys.c
    core/os.c
    core/exception_handlers.c
    core/synchro.c
    core/tcb.c

    drivers/io.c
    drivers/io_list.c
    drivers/comm.c
    drivers/timers.c

)

target_link_libraries(jocktos PUBLIC 
    bitmap_allocator
)

target_include_directories(jocktos PUBLIC 
    core
    drivers
    ${CMAKE_SOURCE_DIR}/cmsis # TODO: make this conditional based on the board
    ${CMAKE_SOURCE_DIR}/boards/${BOARD}
)