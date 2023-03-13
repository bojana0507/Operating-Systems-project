//
// Created by os on 6/20/22.
//

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {

private:

    size_t powOf2Ceil(size_t x);

    size_t powOf2Floor(size_t x);

    MemoryAllocator(){
        size_t sizeBuddy = powOf2Ceil(((size_t)HEAP_END_ADDR-(size_t)HEAP_START_ADDR)/8);
        freeStart = (Segment*)((char*)HEAP_START_ADDR + sizeBuddy);
        freeStart->size = (size_t)((char*)HEAP_END_ADDR-(char*)freeStart)/MEM_BLOCK_SIZE;
        freeStart->next = nullptr;
        usedStart = nullptr;
    };

public:

    MemoryAllocator(const MemoryAllocator &) = delete;
    MemoryAllocator & operator = (const MemoryAllocator &) = delete;

    struct Segment {
        size_t size;
        Segment* next;
    };

    static Segment* freeStart;
    static Segment* usedStart;

    static MemoryAllocator& instance(){
        static MemoryAllocator singleton;
        return singleton;
    }

    void* alloc(size_t size);

    int free(void* block);

    static size_t toBlocks(uint64 originalBytes);

};

#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
