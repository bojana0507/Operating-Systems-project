//
// Created by os on 2/5/23.
//

#include "../h/MemoryCache.hpp"
#include "../h/MemoryBuddy.hpp"
#include "../h/slab.hpp"
#include "../h/print.hpp"

MemoryCache* MemoryCache::cacheList = nullptr;
MemoryCache* MemoryCache::cacheListBuffers = nullptr;

void MemoryCache::setArgs(const char *cacheName, int slotSize, void (*c)(void *), void (*d)(void *)) {
    cacheError = 0;
    name = cacheName;
    nextCache = cacheList;
    cacheList = this;
    slotSizeBytes = slotSize;
    numSlabs = 0;
    usedSlotNum = 0;
    freeSlabs = nullptr;
    usedSlabs = nullptr;
    fullSlabs = nullptr;
    ctor = c;
    dtor = d;
    setSlabSize(MemoryBuddy::expMin);//+(MemoryBuddy::expMax-MemoryBuddy::expMin)/4);
}

void MemoryCache::addToList(MemorySlab **header, MemorySlab* slab) {
    slab->nextSlab = *header;
    *header = slab;
}

void MemoryCache::removeFromList(MemorySlab **header, MemorySlab* slab) {
    if (slab == *header) *header = slab->nextSlab;
    else {
        MemorySlab *prev = *header, *curr = prev->nextSlab;
        while (curr != slab) { prev = curr; curr = curr->nextSlab; }
        prev->nextSlab = curr->nextSlab;
    }
}

void MemoryCache::cacheInfoKernel() {
    printStringCons("\nCACHE INFO:\n");
    printStringCons("Cache name: ");
    printStringCons(name);
    printStringCons("\nSlot size[bytes]: ");
    printIntegerCons(slotSizeBytes);
    printStringCons("\nCache size[blocks]: ");
    printIntegerCons((1<<(slabSizeExp-MemoryBuddy::expMin))*numSlabs + 1);
    printStringCons("\nNum of slabs: ");
    printIntegerCons(numSlabs);
    printStringCons("\nNum of slots per slab: ");
    printIntegerCons(slotNumPerSlab);
    printStringCons("\nCache occupacy: ");
    if (numSlabs==0) printIntegerCons(0);
    else printIntegerCons(usedSlotNum*100/(slotNumPerSlab*numSlabs));
    printStringCons("% \n");
    //check
    printStringCons("Used slots: ");
    printIntegerCons((size_t)usedSlotNum);
//    printStringCons("\nFull slabs: ");
//    printIntegerCons((size_t)fullSlabs);
//    printStringCons("\nUsed slabs: ");
//    printIntegerCons((size_t)usedSlabs);
//    printStringCons("\n");
    //if (usedSlabs) usedSlabs->printBitvector(this);
}

void MemoryCache::cacheInfo() {
    printString("\nCACHE INFO:\n");
    printString("Cache name: ");
    printString(name);
    printString("\nSlot size[bytes]: ");
    printInt(slotSizeBytes);
    printString("\nCache size[blocks]: ");
    printInt((1<<(slabSizeExp-MemoryBuddy::expMin))*numSlabs + 1);
    printString("\nNum of slabs: ");
    printInt(numSlabs);
    printString("\nNum of slots per slab: ");
    printInt(slotNumPerSlab);
    printString("\nCache occupacy: ");
    if (numSlabs==0) printInt(0);
    else printInt(usedSlotNum*100/(slotNumPerSlab*numSlabs));
    printString("% \n");
    //check
//    printString("Used slots: ");
//    printInt((size_t)usedSlotNum);
//    printString("\nFull slabs: ");
//    printInt((size_t)fullSlabs);
//    printString("\nUsed slabs: ");
//    printInt((size_t)usedSlabs);
//    printString("\n");
    //if (usedSlabs) usedSlabs->printBitvector(this);
}

MemorySlab* MemoryCache::newSlab() {
    MemorySlab* slab = (MemorySlab*)MemoryBuddy::alloc(slabSizeExp);
    if (!slab) return slab;
    //printIntCons((size_t)slab); //check
    slab->setArgs(this);
    numSlabs++;
    return slab;
}

int MemoryCache::shrinkCache() {
    int numBlocksFreed = 0;
    for (MemorySlab* curr = freeSlabs, *next; curr; curr = next) {
        next = curr->nextSlab;
        MemoryBuddy::free(curr, slabSizeExp);
        numSlabs--;
        numBlocksFreed += 1<<(slabSizeExp-MemoryBuddy::expMin);
    }
    freeSlabs = nullptr;
    return numBlocksFreed;
}

size_t MemoryCache::getSlabHeaderSize() {
    int bitvecSize = (slotNumPerSlab / 8) + ((slotNumPerSlab % 8)!=0);
    return sizeof(void*)*2 + bitvecSize;
}

void MemoryCache::setSlabSize(int expMax) {
    //int expMax = MemoryBuddy::expMin+(MemoryBuddy::expMax-MemoryBuddy::expMin)/4;
    slabSizeExp = expMax;
    size_t slabSize = 1 << slabSizeExp;
    slotNumPerSlab = slabSize / slotSizeBytes;
    while (slabSize-getSlabHeaderSize() < (size_t)(slotNumPerSlab*slotSizeBytes)) slotNumPerSlab--;
    size_t minFrag = (slabSize - getSlabHeaderSize()) % slotSizeBytes;

    for (int exp = MemoryBuddy::expMin; exp < expMax; exp++){
        slabSize = 1 << exp;
        if (slabSize < (size_t)slotSizeBytes) continue;
        slotNumPerSlab = slabSize / slotSizeBytes;
        while (slabSize-getSlabHeaderSize() < (size_t)(slotNumPerSlab*slotSizeBytes)) slotNumPerSlab--;
        size_t frag = (slabSize - getSlabHeaderSize()) % slotSizeBytes;

        if (frag < minFrag){
            minFrag = frag;
            slabSizeExp = exp;
        }
    }
    slabSize = 1 << slabSizeExp;
    slotNumPerSlab = slabSize / slotSizeBytes;
    while (slabSize-getSlabHeaderSize() < (size_t)(slotNumPerSlab*slotSizeBytes)) slotNumPerSlab--;
}

void *MemoryCache::allocSlot() {
    MemorySlab* slab;
    if (usedSlabs) slab = usedSlabs;
    else if (freeSlabs) {
        slab = freeSlabs;
        removeFromList(&freeSlabs, slab);
        addToList(&usedSlabs, slab);
    }
    else {
        //printStringCons("\nAlloc new slab\n");
        slab = newSlab();
        if (!slab) return slab;
        addToList(&usedSlabs, slab);
        //printStringCons("Added to list\n");
    }
    usedSlotNum++;
    return slab->allocSlot(this);
}

int MemoryCache::freeSlot(void *slot) {
    MemorySlab* slab;
    for (slab = usedSlabs; slab; slab = slab->nextSlab) {
        if (slab < slot && (slab+(1<<slabSizeExp)) > slot) {
            //printStringCons("\nFound slot in used slab.\n");
            usedSlotNum--;
            return slab->freeSlot(this, slot);
        }
    }
    for (slab = fullSlabs; slab; slab = slab->nextSlab) {
        if (slab < slot && (slab+(1<<slabSizeExp)) > slot) {
            //printStringCons("\nFound slot in full slab.\n");
            usedSlotNum--;
            removeFromList(&(fullSlabs), slab);
            addToList(&(usedSlabs), slab);
            return slab->freeSlot(this, slot);
        }
    }
    return -1;  //not in any of allocated slabs in this cache
}

void MemoryCache::destroyCache(MemoryCache** list) {
    freeAllSlabs(freeSlabs);
    freeAllSlabs(usedSlabs);
    freeAllSlabs(fullSlabs);
    if (this == *list) *list = nextCache;
    else {
        MemoryCache* cache;
        for (cache = *list; cache->nextCache != this; cache = cache->nextCache);
        cache->nextCache = nextCache;
    }
    MemoryBuddy::free(this, MemoryBuddy::expMin);
}

void MemoryCache::freeAllSlabs(MemorySlab *header) {
    for (MemorySlab* curr = header, *next; curr; curr = next) {
        next = curr->nextSlab;
        if (dtor) curr->dtorSlab(this);
        MemoryBuddy::free(curr, slabSizeExp);
    }
}

MemoryCache *MemoryCache::getBufferCache(int expSize) {
    MemoryCache* prev = nullptr, *cache;
    int slotSize = 1 << expSize;
    for (cache = cacheListBuffers; cache; cache = cache->nextCache) {
        if (cache->slotSizeBytes > slotSize) break;
        if (cache->slotSizeBytes == slotSize) return cache;
        prev = cache;
    }
//    printStringCons("\nnew cacheN");
//    printIntegerCons(expSize);
//    printIntegerCons((size_t)cacheListBuffers);
//    printStringCons("\n");
    MemoryCache* newCache = (kmem_cache_t*)MemoryBuddy::alloc(MemoryBuddy::expMin);
    newCache->setBufferArgs("size_N", expSize);
    newCache->nextCache = cache;
    if (prev) prev->nextCache = newCache;
    else cacheListBuffers = newCache;
    return newCache;
}

void MemoryCache::setBufferArgs(const char *cacheName, int expSlotSize, void (*c)(void *), void (*d)(void *)) {
    cacheError = 0;
    name = cacheName;
    slotSizeBytes = 1 << expSlotSize;
    numSlabs = 0;
    usedSlotNum = 0;
    freeSlabs = nullptr;
    usedSlabs = nullptr;
    fullSlabs = nullptr;
    ctor = c;
    dtor = d;
    int expSlabSize = (expSlotSize >= MemoryBuddy::expMin) ? (expSlotSize + 1) : MemoryBuddy::expMin;
    setSlabSize(expSlabSize);
}

int MemoryCache::freeBufferSlot(const void *slot) {
    for (MemoryCache* cache = cacheListBuffers; cache; cache = cache->nextCache) {
        if (!cache->freeSlot(const_cast<void*>(slot))) {
            //if (!cache->usedSlabs && !cache->fullSlabs) cache->destroyCache(&cacheListBuffers); //optional, cache destroy can be in main.cpp too
            return 0;
        }
    }
    return -1;
}

