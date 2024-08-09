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
    void* head;     ///< Pointer to the start of the memory block.
    uint16_t size;  ///< Size of the memory block.
} MemoryBlock;

/**
 * @brief Bitmaps for tracking used and allocated memory.
 */
typedef struct {
    uint16_t* used;   ///< Bitmap tracking used blocks.
    uint16_t* heads;  ///< Bitmap tracking allocated block heads.
    uint16_t size;    ///< Size of the bitmap.
} BitMaps;

/**
 * @brief Represents an allocator with bitmaps and a memory block.
 */
typedef struct {
    BitMaps bitmaps;      ///< Bitmaps for managing memory allocation.
    MemoryBlock memory;   ///< The memory block being managed.
    uint16_t block_size;  ///< Size of each memory block.
} Allocator;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
 * @brief Initializes an allocator.
 * 
 * @param allocator The allocator to initialize.
 * @param block_size The size of each block.
 * @param memory The memory region to manage.
 * @param size The size of the memory region.
 * 
 * @note
 * The provided `memory` MUST point to a block of free, zero-initialized memory of size `size`.
 */
void initAllocator(Allocator* allocator, uint16_t block_size, void* memory, uint16_t size);

/**
 * @brief Allocates a block of memory from the allocator.
 * 
 * @param allocator The allocator to use for allocation.
 * @param size The size of the memory block to allocate in bytes.
 * @return A pointer to the allocated memory block, or NULL if the space is unavailable.
 */
void* allocate(Allocator* allocator, uint16_t size);

/**
 * @brief Deallocates a previously allocated block of memory from the allocator.
 *
 * @param allocator The allocator to use for deallocation.
 * @param ptr A pointer to the start of the block of memory to be deallocated.
 *
 * @return true if the block was successfully deallocated, false otherwise.
 */
bool deallocate(Allocator* allocator, void* ptr);

#endif // _ALLOCATOR_H_
