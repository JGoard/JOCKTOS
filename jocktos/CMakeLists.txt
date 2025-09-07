cmake_minimum_required(VERSION 3.21)
include(FetchContent)

function(fetch_github_repository target_name github_username)
    if(NOT TARGET ${target_name})
        message(STATUS "Fetching ${target_name}...")
        FetchContent_Declare(
            ${target_name}
            GIT_REPOSITORY https://github.com/${github_username}/${target_name}.git
            GIT_TAG master
        )
        FetchContent_MakeAvailable(${target_name})
    endif()

    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "${target_name} not found")
    endif()
endfunction()

fetch_github_repository(
    bitmap_allocator 
    nick-a-schneider)

fetch_github_repository(
    buffers 
    nick-a-schneider)

# Build the static library
add_library(jocktos 
    core/sys.c
    core/os.c
    core/os_tasks.c
    core/exception_handlers.c
    core/synchro.c
    core/tcb.c
    
    drivers/io.c
    drivers/io_list.c
    drivers/comm.c
    drivers/timers.c

)

set(USE_BITMAP_ALLOCATOR ON CACHE BOOL "Use bitmap allocator for buffer allocation")
target_link_libraries(jocktos PUBLIC 
    bitmap_allocator
    buffers
)

target_include_directories(jocktos PUBLIC 
    core
    drivers
    ${CMAKE_SOURCE_DIR}/cmsis # TODO: make this conditional based on the board
    ${CMAKE_SOURCE_DIR}/boards/${BOARD}
)
