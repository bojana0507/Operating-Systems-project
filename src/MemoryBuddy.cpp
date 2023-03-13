//
// Created by os on 2/3/23.
//

#include "../h/MemoryBuddy.hpp"

size_t MemoryBuddy::sizeMax = 0;
size_t MemoryBuddy::sizeMin = 0;
int MemoryBuddy::expMax = 0;
int MemoryBuddy::expMin = 0;
size_t** MemoryBuddy::freeLists = nullptr;

MemoryBuddy::MemoryBuddy() {
    sizeMax = (size_t)MemoryAllocator::instance().freeStart - (size_t)HEAP_START_ADDR;
    sizeMin = BLOCK_SIZE;
    size_t size = sizeMax;
    int n = 0;
    while(size >= sizeMin) { n++; size >>= 1; }
    expMin = 12; //BLOCK_SIZE=4096
    expMax = n + expMin - 1;
    freeLists = (size_t**)MemoryAllocator::instance().alloc(MemoryAllocator::toBlocks(n*sizeof(size_t*)));
    for (int i = 0; i < n-1; i++) {
        freeLists[i] = nullptr;
    }
    freeLists[n-1] = (size_t*)HEAP_START_ADDR;
    *(freeLists[n-1]) = 0;
}

void *MemoryBuddy::alloc(int i) {
    if (i<expMin || i>expMax) return 0; //Not a valid 2^i!
    if (freeLists[i-expMin]){
        size_t* ret = (size_t*)freeLists[i-expMin];
        freeLists[i-expMin] = (size_t*)*ret;
        return ret;
    }
    for (int j=i+1; j<=expMax; j++){
        size_t* seg1 = freeLists[j-expMin];
        if (seg1){
            size_t* seg2 = (size_t*)((size_t)seg1 + (1<<(j-1)));
            freeLists[j-expMin] = (size_t*)*seg1;
            *seg2 = (size_t)freeLists[j-expMin-1]; //=nullptr
            *seg1 = (size_t)seg2;
            freeLists[j-expMin-1] = seg1;
            return alloc(i);
        }
    }
    return 0; // No memory!
}

void MemoryBuddy::free(void *segm, int i) { //DOESN'T CHECK SEGM ADDR
    if (i<expMin || i>expMax) return; //Not a valid 2^i!
    size_t* segment = (size_t*) segm;
    for (int j = i; j <= expMax; j++){
        segment = merge(segment, j);
        if (!segment) break; //no more merging
    }
}

size_t* MemoryBuddy::merge(size_t *segment, int i) {
    if (i<expMin || i>expMax) return 0; //Not a valid 2^i!
    size_t* buddy = 0;
    bool isLeftBuddy = ((((size_t)segment-(size_t)HEAP_START_ADDR) % (1<<(i+1))) == 0);
    if (isLeftBuddy) {
        buddy = (size_t*)((char*)segment + (1<<i));
        size_t* prev = nullptr, *next = freeLists[i-expMin];
        for (; segment < next; next = (size_t*)*next) {
            if (next == buddy) { //connect prev and *next, test for level above
                if (prev) *prev = *next;
                else freeLists[i-expMin] = (size_t*)*next;
                return segment;
            }
            prev = next;
        }
        *segment = (size_t)next;
        if (prev) *prev = (size_t)segment;
        else freeLists[i-expMin] = segment;
        return 0;
    }
    else {//segment isRightBuddy
        buddy = (size_t*)((char*)segment - (1<<i));
        size_t* prev = nullptr, *next = freeLists[i-expMin];
        for (; segment < next || next == buddy; next = (size_t*)*next) {
            if (next == buddy) { //connect prev and *next, test for level above
                if (prev) *prev = *next;
                else freeLists[i-expMin] = (size_t*)*next;
                return buddy;
            }
            prev = next;
        }
        *segment = (size_t)next;
        if (prev) *prev = (size_t)segment;
        else freeLists[i-expMin] = segment;
        return 0;
    }
}