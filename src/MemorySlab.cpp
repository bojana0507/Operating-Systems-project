//
// Created by os on 2/3/23.
//

#include "../h/MemorySlab.hpp"
#include "../h/MemoryCache.hpp"
#include "../h/MemoryBuddy.hpp"
#include "../h/print.hpp"

void MemorySlab::setArgs(MemoryCache* cache) {
    freeSlotP = (char*)this + cache->getSlabHeaderSize();
//    printString("\nfree slot:\n");
//    printInt((size_t)freeSlotP);
//    printString("\nbitvec slot:\n");
//    printInt((size_t)&bitvec);
//    printString("\n");
    void* slot = freeSlotP;
    for (int i = 0; i < (cache->slotNumPerSlab/vecElemSize+(cache->slotNumPerSlab%vecElemSize!=0)); i++){
        *(&bitvec+i) = 0;
        if(cache->ctor != nullptr){
            for (int j = 0; j < vecElemSize; j++) {
                if (i*vecElemSize+j >= cache->slotNumPerSlab) break;
                (cache->ctor)(slot);
                slot = (char *) slot + cache->slotSizeBytes;
            }
        }
    }
    nextSlab = nullptr;
}

void *MemorySlab::allocSlot(MemoryCache* cache) {
    void* ret = freeSlotP;
    int num = ((char*)freeSlotP - (char*)this - cache->getSlabHeaderSize())/(cache->slotSizeBytes);
    int ind = num / vecElemSize;
    *(&bitvec+ind) |= ((uint8)1)<<(vecElemSize-1-num%vecElemSize);
    bool firstCheck = true;
    int lastByteInd = cache->slotNumPerSlab/vecElemSize+(cache->slotNumPerSlab%vecElemSize!=0);
    uint8 lastByteMax = vecMaxElem;
    if (cache->slotNumPerSlab%vecElemSize) lastByteMax <<= (vecElemSize - cache->slotNumPerSlab%vecElemSize);
    for (int i = ind; i != ind || firstCheck; i = (i+1)%lastByteInd){
        firstCheck = false;
        if ((*(&bitvec+i) != vecMaxElem && i!=(lastByteInd-1)) || (*(&bitvec+i) != lastByteMax && i==(lastByteInd-1))) {
            int bit = i*vecElemSize;
            uint8 mask = 1<<(vecElemSize-1);
            int max = vecElemSize;
            if (i==(lastByteInd-1) && cache->slotNumPerSlab%vecElemSize) { //check if it's last byte of bitvector
                max = cache->slotNumPerSlab%vecElemSize;
            }
            for (int j = 0; j < max; j++){
                if(!(mask & (*(&bitvec+i)))) {
                    bit += j;
                    break;
                }
                mask>>=1;
            }
            freeSlotP = (char*)this + cache->getSlabHeaderSize() + bit*cache->slotSizeBytes;
            return ret;
        }
    }
    freeSlotP = nullptr;
    cache->removeFromList(&(cache->usedSlabs), this);
    cache->addToList(&(cache->fullSlabs), this);
    return ret;
}

int MemorySlab::freeSlot(MemoryCache* cache, void* slot) {
    int num = ((char*)slot - (char*)this - cache->getSlabHeaderSize())/(cache->slotSizeBytes);
    if (((char*)slot - (char*)this - cache->getSlabHeaderSize())%(cache->slotSizeBytes)) {
        return -1; //not a valid slot address
    }
    int ind = num / vecElemSize;
    *(&bitvec+ind) &= ~(((uint8)1)<<(vecElemSize-1-num%vecElemSize));
    if (!freeSlotP) freeSlotP = slot;   //if it was full earlier
    for (int i = 0; i < (cache->slotNumPerSlab/vecElemSize+(cache->slotNumPerSlab%vecElemSize!=0)); i++){
        if (*(&bitvec+i) != 0) return 0;   //not empty
    }
    cache->removeFromList(&(cache->usedSlabs), this);
    cache->addToList(&(cache->freeSlabs), this);
    return 0;
}

void MemorySlab::printBitvector(MemoryCache *cache) {
    int lastByteInd = cache->slotNumPerSlab/vecElemSize+(cache->slotNumPerSlab%vecElemSize!=0);
    for (int i = 0; i < lastByteInd; i++) {
        printStringCons("\nBYTE");
        printIntegerCons(i);
        printStringCons(": ");
        uint8 mask = 1<<(vecElemSize-1);
        int max = vecElemSize;
        if (i==(lastByteInd-1) && cache->slotNumPerSlab%vecElemSize) { //check if it's last byte of bitvector
            max = cache->slotNumPerSlab%vecElemSize;
        }
        for (int j = 0; j < max; j++){
            printIntegerCons((mask & (*(&bitvec+i)))!=0);
            mask>>=1;
        }
    }
}

void MemorySlab::dtorSlab(MemoryCache* cache) {
    void* slot = (char*)this + cache->getSlabHeaderSize();
    for (int i = 0; i < (cache->slotNumPerSlab/vecElemSize+(cache->slotNumPerSlab%vecElemSize!=0)); i++){
        for (int j = 0; j < vecElemSize; j++) {
            if (i*vecElemSize+j >= cache->slotNumPerSlab) break;
            (cache->dtor)(slot);
            slot = (char *) slot + cache->slotSizeBytes;
        }
    }
}
