/**
* \brief This header is to act as companion header for allocator.c
*/
#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
// Bios
// Standard C
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* -- Defines ------------------------------------------------------------- */
#define ALLOCATOR_SIZE 8192 // TODO: redefine this to use full heap as per the linker file
/* -- Types --------------------------------------------------------------- */

/**
 * @brief A block of memory with a pointer to its head and its size.
 */
typedef struct {
    void* head;     ///< Pointer to the start of the memory block
    size_t size;    ///< Size of the memory block
} MemoryBlock;

/**
 * @brief Bitmaps for tracking used and allocated memory.
 */
typedef struct {
    uint64_t* used;   ///< Bitmap tracking used blocks
    uint64_t* alloc;  ///< Bitmap tracking allocated blocks
    size_t size;      ///< Size of the bitmap
} BitMaps;

/**
 * @brief Represents an allocator with bitmaps and a memory block.
 */
typedef struct {
    BitMaps bitmaps;      ///< Bitmaps for managing memory allocation
    MemoryBlock memory;   ///< The memory block being managed
    size_t blockSize;     ///< Size of each memory block
} Allocator;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
 * @brief Initializes an allocator.
 * 
 * @param allocator The allocator to initialize
 * @param memory The memory region to manage
 * @param size The size of the memory region
 * @param blockSize The size of each block
 */
void initAllocator(Allocator* allocator, void* memory, size_t size, size_t blockSize);

/**
 * @brief Allocates a block of memory.
 * 
 * @param allocator The allocator to use for allocation
 * @param size The size of the memory block to allocate
 * @return A pointer to the allocated memory, or NULL if allocation fails
 */
void* allocate(Allocator* allocator, size_t size);

/**
 * @brief Deallocates a previously allocated block of memory.
 * 
 * @param allocator The allocator to use for deallocation
 * @param ptr A pointer to the memory block to deallocate
 * @return True if deallocation is successful, false otherwise
 */
bool deallocate(Allocator* allocator, void* ptr);

#endif // _ALLOCATOR_H_
