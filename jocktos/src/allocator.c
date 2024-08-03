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
void setBit(uint64_t* bitmap, size_t index);

/**
 * @brief Sets the indexed bit to false.
 * 
 * @param bitmap The bitmap to be modified
 * @param index The index of the bit to be disabled
 */
void clearBit(uint64_t* bitmap, size_t index);

/**
 * @brief Get the value of the indexed bit.
 * 
 * @param bitmap The bitmap to be sampled
 * @param index The index of the bit to be sampled
 */
bool getBit(uint64_t* bitmap, size_t index);

/**
 * @brief Finds a contiguous sequence of free blocks.
 * 
 * @param used The bitmap representing used blocks
 * @param size The size of the bitmap
 * @param numBlocks The number of contiguous blocks needed
 * @return The index of the first block in the contiguous sequence if found, or -1 if not found
 */
size_t findContiguousFreeBlocks(uint64_t* used, size_t size, size_t numBlocks);

/* -- Public Functions----------------------------------------------------- */

// Allocator functions
void initAllocator(Allocator* allocator, void* memory, size_t size, size_t blockSize) {
    size_t numBlocks = size / blockSize;
    size_t bitmapSize = (numBlocks + 63) / 64 * 2 * sizeof(uint64_t);
    allocator->bitmaps.size = numBlocks;
    allocator->bitmaps.used = (uint64_t*)memory;
    memory = (void*)((uintptr_t)memory + bitmapSize / 2);
    allocator->bitmaps.alloc = (uint64_t*)memory;
    memory = (void*)((uintptr_t)memory + bitmapSize / 2);
    allocator->memory.head = memory;
    allocator->memory.size = size - bitmapSize;
    allocator->blockSize = blockSize;
}

void* allocate(Allocator* allocator, size_t size) {
    size_t numBlocks = (size + allocator->blockSize - 1) / allocator->blockSize;
    size_t startIndex = findContiguousFreeBlocks(allocator->bitmaps.used, allocator->bitmaps.size, numBlocks);
    if (startIndex == SIZE_MAX) {
        return NULL;
    }
    for (size_t i = 0; i < numBlocks; i++) {
        setBit(allocator->bitmaps.used, startIndex + i);
    }
    setBit(allocator->bitmaps.alloc, startIndex);
    return (void*)((uintptr_t)allocator->memory.head + startIndex * allocator->blockSize);
}

bool deallocate(Allocator* allocator, void* ptr) {
    size_t index = ((uintptr_t)ptr - (uintptr_t)allocator->memory.head) / allocator->blockSize;
    if (!getBit(allocator->bitmaps.alloc, index)) {
        return false;
    }
    clearBit(allocator->bitmaps.alloc, index);
    while (getBit(allocator->bitmaps.used, index) && !getBit(allocator->bitmaps.alloc, index)) {
        clearBit(allocator->bitmaps.used, index++);
        if (index >= allocator->bitmaps.size) break;
    }
    return true;
}


/* -- Private Functions --------------------------------------------------- */

size_t findContiguousFreeBlocks(uint64_t* used, size_t size, size_t numBlocks) {
    size_t count = 0;
    for (size_t i = 0; i < size; i++) {
        if (!getBit(used, i)) {
            count++;
            if (count == numBlocks) {
                return i - numBlocks + 1;
            }
        } else {
            count = 0;
        }
    }
    return SIZE_MAX;
}

void setBit(uint64_t* bitmap, size_t index) {
    bitmap[index / 64] |= (1ULL << (index % 64));
}

void clearBit(uint64_t* bitmap, size_t index) {
    bitmap[index / 64] &= ~(1ULL << (index % 64));
}

bool getBit(uint64_t* bitmap, size_t index) {
    return (bitmap[index / 64] & (1ULL << (index % 64))) != 0;
}