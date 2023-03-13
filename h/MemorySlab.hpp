//
// Created by os on 2/3/23.
//

#ifndef PROJECT_BASE_V1_1_MEMORYSLAB_HPP
#define PROJECT_BASE_V1_1_MEMORYSLAB_HPP

#define vecElemSize (8)
#define vecMaxElem (0xFFu)

#include "../lib/hw.h"

class MemoryCache;

class MemorySlab {

    friend class MemoryCache;

private:
    MemorySlab* nextSlab;
    void* freeSlotP;
    uint8 bitvec;

public:

    void dtorSlab(MemoryCache* cache);

    void setArgs(MemoryCache* cache);

    void* allocSlot(MemoryCache* cache);

    int freeSlot(MemoryCache* cache, void* slot);

    void printBitvector(MemoryCache* cache); //debug
};


#endif //PROJECT_BASE_V1_1_MEMORYSLAB_HPP
