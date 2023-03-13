//
// Created by os on 6/20/22.
//

#include "../h/MemoryAllocator.hpp"

#include "../h/print.hpp"
//#include "../lib/mem.h"

MemoryAllocator::Segment* MemoryAllocator::freeStart = nullptr;
MemoryAllocator::Segment* MemoryAllocator::usedStart = nullptr;

void *MemoryAllocator::alloc(size_t size) {

    /*searching for a big enough block*/
    Segment* newUsed, *newFree, *prevFree;
    bool foundFree = false;
    for (newUsed = freeStart, prevFree = nullptr; newUsed; newUsed = newUsed->next) {
        if (newUsed->size >= size) {
            foundFree = true;
            break;
        }
        prevFree = newUsed;
    }
    if (!foundFree) {
        return nullptr; //no free segment is big enough
    }

    /*free segments reconnecting - not the whole free segment is needed*/
    if (newUsed->size > size){
        newFree =(Segment*)((char*)newUsed + size*MEM_BLOCK_SIZE);
        newFree->size = newUsed->size - size;
        if (prevFree) prevFree->next = newFree;
        else freeStart = newFree;
        newFree->next = newUsed->next;
    }
    /*free segments reconnecting - the whole free segment is needed*/
    else {
        if (prevFree) prevFree->next = newUsed->next;
        else freeStart = newUsed->next;

    }
    /*used segments reconnecting*/
    Segment* nextUsed, *prevUsed;
    for (nextUsed = usedStart, prevUsed = nullptr; nextUsed < newUsed && nextUsed; nextUsed = nextUsed->next) prevUsed = nextUsed;
    newUsed->next = nextUsed;
    if (prevUsed) prevUsed->next = newUsed;
    else usedStart = newUsed;

    newUsed->size = size;

    return ((char*)newUsed + sizeof(Segment)); //return the info without the header
}

int MemoryAllocator::free(void *block) {
    block = (Segment*)((char*)block - sizeof(Segment));
    if (block < HEAP_START_ADDR || block > HEAP_END_ADDR) return -1; //pointer block is out of HEAP range
    Segment *newFree, *prevUsed;
    for (newFree = usedStart, prevUsed = nullptr; newFree; newFree = newFree->next) {
        if (newFree == block) break;
        if (newFree > block) return -2; //pointer block is in the middle of an allocated segment
        prevUsed = newFree;
    }
    if (!newFree) return -3; //pointer block isn't allocated yet

    /*used segments reconnecting*/
    if (prevUsed) prevUsed->next = newFree->next;
    else usedStart = newFree->next;

    /*free segments reconnecting*/
    Segment *nextFree, *prevFree;
    for (nextFree = freeStart, prevFree = nullptr; (nextFree < newFree) && nextFree; nextFree = nextFree->next) prevFree = nextFree;
    if (prevFree && ((char*)prevFree + prevFree->size * MEM_BLOCK_SIZE == (char*)newFree)) { //append to the previous free block
        prevFree->size += newFree->size;
        if (nextFree && ((char*)prevFree + prevFree->size *MEM_BLOCK_SIZE == (char*)nextFree)) { //append the next block too
            prevFree->size += nextFree->size;
            prevFree->next = nextFree->next;
        }
        return 0;
    } else if (nextFree && ((char*)newFree + newFree->size * MEM_BLOCK_SIZE == (char*)nextFree)) { //append to the next free block
        newFree->size += nextFree->size;
        newFree->next = nextFree->next;
        if (prevFree) prevFree->next = newFree;
        else freeStart = newFree;
        return 0;
    }
    newFree->next = nextFree;
    if (prevFree) prevFree->next = newFree;
    else freeStart = newFree;
    return 0;
}

size_t MemoryAllocator::toBlocks(uint64 originalBytes) {
    return (originalBytes + sizeof(Segment) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
}

size_t MemoryAllocator::powOf2Ceil(size_t x) {
    if (x <= 1) return 1;
    int power = 2;
    x--;
    while (x >>= 1) power <<= 1;
    return power;
}

size_t MemoryAllocator::powOf2Floor(size_t x) {
    int power = 1;
    while (x >>= 1) power <<= 1;
    return power;
}