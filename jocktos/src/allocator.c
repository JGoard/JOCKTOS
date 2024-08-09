/**
* \brief This module defines the custom bitmap allocator used by the OS
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "allocator.h"
// Middleware
// Bios
// Standard C
#include <memory.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */

/**
 * @brief Sets the indexed bit to true.
 * 
 * @param bitmap The bitmap to be modified
 * @param index The index of the bit to be enabled
 */
void setBit(uint16_t* bitmap, uint16_t index);

/**
 * @brief Sets the indexed bit to false.
 * 
 * @param bitmap The bitmap to be modified
 * @param index The index of the bit to be disabled
 */
void clearBit(uint16_t* bitmap, uint16_t index);

/**
 * @brief Get the value of the indexed bit.
 * 
 * @param bitmap The bitmap to be sampled
 * @param index The index of the bit to be sampled
 */
bool getBit(uint16_t* bitmap, uint16_t index);

/**
 * @brief Finds a contiguous sequence of free blocks.
 *
 * @details
 * This function takes a bitmap representing used blocks, the size of the bitmap, and the number of contiguous blocks needed.
 * It iterates through each bit in the bitmap, keeping track of the count of consecutive free blocks.
 * When it finds a sequence of `num_blocks` consecutive free blocks, it returns the index of the first block in the sequence.
 * If it reaches the end of the bitmap without finding a suitable sequence, it returns SIZE_MAX.
 *
 * @param num_blocks The number of contiguous blocks needed
 * @param used The bitmap representing used blocks
 * @param size The size of the bitmap
 * @return The index of the first block in the contiguous sequence if found, or UINT16_MAX if not found
 */
uint16_t findContiguousFreeBlocks(uint16_t num_blocks, uint16_t* used, uint16_t size);

/* -- Public Functions----------------------------------------------------- */

/**
 * @details
 * This function initializes an allocator with the provided parameters.
 * The memory region pointed to by `memory` is divided into two parts:
 * - A portion of the memory will be dedicated to the bitmaps. The size of this portion 
 *   is calculated based on the number of blocks that can fit in the provided memory.
 * - The other portion of the memory will be used to store the allocated blocks.
 */
void initAllocator(Allocator* allocator, uint16_t block_size, void* memory, uint16_t size) {
    // Calculate the number of blocks that can fit in the provided memory
    uint16_t num_blocks = size / block_size;
    // Calculate the size of the bitmap portion of the memory region
    uint16_t bitmap_size = ((num_blocks + 15) / 16) * 2 * sizeof(uint16_t);
    // Initialize the allocator with the calculated values
    allocator->bitmaps.size = (size - bitmap_size) / block_size;  // Number of blocks in the memory region
    allocator->bitmaps.used = (uint16_t*)memory;  // Pointer to the used bitmap
    // Adjust the memory pointer to the start of the allocated block portion
    memory = (void*)((uint8_t*)memory + bitmap_size / 2);
    allocator->bitmaps.heads = (uint16_t*)memory;  // Pointer to the allocated bitmap
    // Adjust the memory pointer to the start of the allocated block portion
    memory = (void*)((uint8_t*)memory + bitmap_size / 2);
    allocator->memory.head = memory;  // Pointer to the start of the allocated block portion
    allocator->memory.size = allocator->bitmaps.size * block_size;  // Size of the allocated block portion
    allocator->block_size = block_size;  // Size of each block
}

/**
 * @details
 * This function finds a contiguous sequence of free blocks in the allocator's bitmap and marks them as used.
 * It then returns a pointer to the start of the allocated block.
 * If no contiguous free blocks are available, it returns NULL.
 */
void* allocate(Allocator* allocator, uint16_t size) {
    // Calculate the number of blocks needed to allocate the requested size
    uint16_t num_blocks = (size + allocator->block_size - 1) / allocator->block_size;
    // Find the index of the first contiguous free block in the bitmap
    uint16_t start_index = findContiguousFreeBlocks(num_blocks, allocator->bitmaps.used, allocator->bitmaps.size);
    // If no contiguous free blocks are available, return NULL
    if (start_index == UINT16_MAX) {
        return NULL;
    }
    // Mark the allocated blocks as used in the bitmap
    for (uint16_t i = 0; i < num_blocks; i++) {
        setBit(allocator->bitmaps.used, start_index + i);
    }
    // Mark the allocated blocks as allocated in the bitmap
    setBit(allocator->bitmaps.heads, start_index);
    // Return a pointer to the head of the allocated block
    return (void*)((uint8_t*)allocator->memory.head + start_index * allocator->block_size);
}

/**
 * @details
 * This function takes a pointer to the start of the block of memory to be deallocated and the allocator from which it was allocated.
 * It calculates the index of the block in the allocator's memory and verifies that the block is currently allocated.
 * It then clears the allocated bit for the block and all subsequent blocks in the bitmap.
 * It returns true if the deallocation was successful, false otherwise.
 */
bool deallocate(Allocator* allocator, void* ptr) {
    // Calculate the index of the block in the allocator's memory
    uint16_t index = ((uint8_t*)ptr - (uint8_t*)allocator->memory.head) / allocator->block_size;
    // Check if the block is currently allocated
    if (!getBit(allocator->bitmaps.heads, index)) return false;
    // Clear the allocated bit for the block
    clearBit(allocator->bitmaps.heads, index);
    // Traverse the bitmap, clearing the used bits for all blocks in the sequence
    while (getBit(allocator->bitmaps.used, index) && !getBit(allocator->bitmaps.heads, index)) {
        clearBit(allocator->bitmaps.used, index++);
        // Break if we reach the end of the bitmap
        if (index >= allocator->bitmaps.size) break;
    }
    return true;
}

/* -- Private Functions --------------------------------------------------- */

uint16_t findContiguousFreeBlocks(uint16_t num_blocks, uint16_t* used, uint16_t size) {
    uint16_t count = 0; // Initialize a counter to track consecutive free blocks
    for (uint16_t i = 0; i < size; i++) { // Iterate through each bit in the bitmap
        if (!getBit(used, i)) { // If the bit is not set (i.e., the block is free)
            count++; // Increment the counter
            if (count == num_blocks) { // If the counter equals the number of blocks needed
                return i - num_blocks + 1; // Return the starting index of the sequence
            }
        } else { // If the bit is set (i.e., the block is used)
            count = 0; // Reset the counter
        }
    }
    return UINT16_MAX; // Return UINT16_MAX if no suitable sequence is found
}

void setBit(uint16_t* bitmap, uint16_t index) {
    bitmap[index / 16] |= (1ULL << (index % 16));
}

void clearBit(uint16_t* bitmap, uint16_t index) {
    bitmap[index / 16] &= ~(1ULL << (index % 16));
}

bool getBit(uint16_t* bitmap, uint16_t index) {
    return (bitmap[index / 16] & (1ULL << (index % 16))) != 0;
}
