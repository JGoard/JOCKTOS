#include "allocator.h"
#include "test_utils.h"
#include <stdbool.h>

// recreation of private function for test purposes
bool get_bit(uint64_t* bitmap, size_t index) {
    return (bitmap[index / 64] & (1ULL << (index % 64))) != 0;
}

void testInitAllocator(void) {

    TEST_CASE("Even multiple of block size") {
        Allocator allocator;
        uint8_t memory[128];
        initAllocator(&allocator, memory, 128, 16);

        ASSERT_EQUAL_INT(allocator.memory.size, 112, "incorrect size after initialization");
        ASSERT_EQUAL_INT(allocator.blockSize, 16, "incorrect block size after initialization");
        ASSERT_EQUAL_INT(allocator.bitmaps.size, 7, "incorrect bitmap size after initialization"); 

        ASSERT_EQUAL_PTR((uint8_t*)(allocator.bitmaps.used), memory, "alloc bitmap placement is wrong"); 
        ASSERT_EQUAL_PTR((uint8_t*)(allocator.bitmaps.alloc), memory + 8, "used bitmap placement is wrong");  
        ASSERT_EQUAL_PTR((uint8_t*)(allocator.memory.head), memory + 16, "alloc bitmap placement is wrong"); 

    } CASE_COMPLETE;

    TEST_CASE("bitmaps fill memory block") {
        // TODO: test for case where 2x 64bit bitmaps exceed the size of the memory block
    } CASE_NOT_IMPLEMENTED;

    TEST_CASE("head aligned correctly") {
        // TODO: verify that the head is aligned correctly
    } CASE_NOT_IMPLEMENTED;

}

void testAllocate() {

    TEST_CASE("allocating entire memory block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, memory, 128, 16);
        void* block = allocate(&allocator, allocator.memory.size);
        ASSERT_EQUAL_PTR(block, allocator.memory.head, "incorrect block head returned");
        void* block2 = allocate(&allocator, 16);
        ASSERT_EQUAL_PTR(block2, NULL, "invalid allocation returned non-null");
        ASSERT_TRUE(get_bit(allocator.bitmaps.alloc, 0), "alloc bit not set");
        for (size_t i = 0; i < allocator.bitmaps.size; i++) {
            ASSERT_TRUE(get_bit(allocator.bitmaps.used, i), "used bit not set");
        }
    } CASE_COMPLETE;
        
    TEST_CASE("allocating impossibly large block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, memory, 128, 16);

        void* block = allocate(&allocator, 1024);
        ASSERT_EQUAL_PTR(block, NULL, "invalid allocation returned non-null");

        for (size_t i = 0; i < allocator.bitmaps.size; i++) {
            ASSERT_FALSE(get_bit(allocator.bitmaps.alloc, i), "alloc bit was set");
            ASSERT_FALSE(get_bit(allocator.bitmaps.used, i),  "used bit was set");
        }
    } CASE_COMPLETE;

    TEST_CASE("allocating block that spans two 64bit words in the bitmap") {
        Allocator allocator;
        uint8_t memory[4096];
        for (int index = 0; index < 4096; index++) memory[index] = 0;
        initAllocator(&allocator, memory, 4096, 16);

        void* block1 = allocate(&allocator, 16 * 75);
        void* block2 = allocate(&allocator, 16);
        ASSERT_NOT_EQUAL_PTR(block1, NULL, "valid allocation returned null");
        ASSERT_NOT_EQUAL_PTR(block2, NULL, "valid allocation returned null");

        ASSERT_TRUE(get_bit(allocator.bitmaps.alloc, 0), "alloc bit not set");
        for (size_t i = 0; i < 75; i++) {
            ASSERT_TRUE(get_bit(allocator.bitmaps.used, i), "used bit not set");
        }
        ASSERT_TRUE(get_bit(allocator.bitmaps.alloc, 75), "alloc bit not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 75), "used bit not set");

    } CASE_COMPLETE;

    TEST_CASE("allocating odd sized block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, memory, 128, 16);

        void* block1 = allocate(&allocator, 17);
        void* block2 = allocate(&allocator, 17);
        ASSERT_NOT_EQUAL_PTR(block1, NULL, "valid allocation returned null");
        ASSERT_NOT_EQUAL_PTR(block2, NULL, "valid allocation returned null");

        ASSERT_EQUAL_PTR((uint8_t*)block1 + 32, (uint8_t*)block2, "blocks not adjacent");

        ASSERT_TRUE(get_bit(allocator.bitmaps.alloc, 0), "alloc bit not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.alloc, 2), "alloc bit not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 0), "used bit not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 1), "used bit not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 2), "used bit not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 3), "used bit not set");

    } CASE_COMPLETE;
}

void testDeallocate() {

    TEST_CASE("deallocating block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, memory, 128, 16);

        void* block = allocate(&allocator, 16);
        ASSERT_TRUE(deallocate(&allocator, block), "deallocation failed");

        ASSERT_FALSE(get_bit(allocator.bitmaps.alloc, 0), "alloc bit was set");
        ASSERT_FALSE(get_bit(allocator.bitmaps.used, 0), "used bit was set");

    } CASE_COMPLETE;

    TEST_CASE("deallocating block that spans two 64bit words in the bitmap") {
        Allocator allocator;
        uint8_t memory[4096];
        for (int index = 0; index < 4096; index++) memory[index] = 0;
        initAllocator(&allocator, memory, 4096, 16);

        void* block1 = allocate(&allocator, 16 * 50);
        void* block2 = allocate(&allocator, 16 * 75);

        ASSERT_TRUE(deallocate(&allocator, block1), "deallocating block1 failed");

        ASSERT_FALSE(get_bit(allocator.bitmaps.alloc, 0), "alloc bit still set after deallocation");
        for (size_t i = 0; i < 50; i++) {
            ASSERT_FALSE(get_bit(allocator.bitmaps.used, i), "used bit still set after deallocation");
        }

        ASSERT_TRUE(get_bit(allocator.bitmaps.alloc, 50), "next allocs bit cleared after deallocation");
        for (size_t i = 0; i < 75; i++) {
            ASSERT_TRUE(get_bit(allocator.bitmaps.used, 50 + i), "next used bit cleared after deallocation");
        }

        ASSERT_TRUE(deallocate(&allocator, block2), "deallocating block2 failed");

    } CASE_COMPLETE;

    TEST_CASE("deallocating invalid block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, memory, 128, 16);
        void* block = (uint8_t*)(allocator.memory.head) + 2;

        ASSERT_FALSE(deallocate(&allocator, block), "invalid block was deallocation without error");
        for (size_t i = 0; i < 8; i++) {
            ASSERT_FALSE(get_bit(allocator.bitmaps.alloc, i), "alloc bit was cleared during invalid deallocation");
        }
    } CASE_COMPLETE;
}

int main(void) {
    TEST_EVAL(testInitAllocator);
    TEST_EVAL(testAllocate);
    TEST_EVAL(testDeallocate);
    return testGetStatus();
}