//
// Created by os on 2/5/23.
//

#ifndef PROJECT_BASE_V1_1_MEMORYCACHE_HPP
#define PROJECT_BASE_V1_1_MEMORYCACHE_HPP

#include "MemorySlab.hpp"
#include "printing.hpp"

class MemoryCache {

    friend class MemorySlab;

private:
    const char* name;
    int slotSizeBytes;
    int slabSizeExp;
    int numSlabs;
    int slotNumPerSlab;
    int usedSlotNum;
    MemorySlab* freeSlabs;
    MemorySlab* usedSlabs;
    MemorySlab* fullSlabs;
    MemoryCache* nextCache;
    void (*ctor)(void *);
    void (*dtor)(void *);
    static MemoryCache* cacheList;
    static MemoryCache* cacheListBuffers;

    void setSlabSize(int expMax); //used only in constructor

    size_t getSlabHeaderSize(); //used only in constructor

    void freeAllSlabs(MemorySlab* header); //used only in destructor

public:

    int cacheError;

    void destroyCache(MemoryCache** list = &cacheList);

    void setArgs(const char* cacheName, int slotSize, void (*ctor)(void *)=nullptr, void (*dtor)(void *)= nullptr);

    void cacheInfoKernel();

    void cacheInfo();

    void addToList(MemorySlab** header, MemorySlab* slab);

    void removeFromList(MemorySlab** header, MemorySlab* slab);

    MemorySlab* newSlab();

    int shrinkCache();

    void* allocSlot();

    int freeSlot(void* slot);

    void setBufferArgs(const char* cacheName, int slotSize, void (*ctor)(void *)=nullptr, void (*dtor)(void *)= nullptr);

    static MemoryCache* getBufferCache(int expSize);

    static int freeBufferSlot(const void* slot);

};


#endif //PROJECT_BASE_V1_1_MEMORYCACHE_HPP
