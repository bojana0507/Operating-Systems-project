//
// Created by os on 2/3/23.
//

#include "../h/slab.hpp"
#include "../h/MemoryBuddy.hpp"

void kmem_init(void *space, int block_num){
    //arguments already defined
    MemoryBuddy::instance();
}

kmem_cache_t *kmem_cache_create(const char *name, size_t size,
                                void (*ctor)(void *),
                                void (*dtor)(void *)){
    kmem_cache_t * cache = (kmem_cache_t*)MemoryBuddy::alloc(MemoryBuddy::expMin);
    if (cache) cache->setArgs(name, size, ctor, dtor);
    return cache;
}

int kmem_cache_shrink(kmem_cache_t *cachep){
    return cachep->shrinkCache();
}

void *kmem_cache_alloc(kmem_cache_t *cachep) {
    void* ret = cachep->allocSlot();
    if (!ret) cachep->cacheError = 1;
//    else {
//        printString("\nAlloc address: ");
//        printInt((size_t)ret);
//        printString("\n");
//    }
    return ret;
}

void kmem_cache_free(kmem_cache_t *cachep, void *objp) {
    int ret = cachep->freeSlot(objp);
    if (ret) cachep->cacheError = 2;
}

void kmem_cache_destroy(kmem_cache_t *cachep) {
    cachep->destroyCache(); //no mistake if we delete a nonempty cache, it deletes all of its slabs and objects
}

void kmem_cache_info(kmem_cache_t *cachep) {
    cachep->cacheInfo();
    kmem_cache_error(cachep);
}

void *kmalloc(size_t size) {
    size_t sizeBytes = 1<<5;
    int exp;
    for (exp = 5; exp <= 17; exp++) {
        if (sizeBytes >= size) break; //zaokruzi
        sizeBytes <<= 1;
    }
//    if (sizeBytes > (1<<17))
//        printString("\nAllocation ERROR: Buffer size invalid.\n");
    MemoryCache* cache = MemoryCache::getBufferCache(exp);
//    if (!cache)
//        printString("\n AllocationBufferCache ERROR: Memory full.\n");
    void* ret = cache->allocSlot();
    if (!ret) cache->cacheError = 3;
//        printString("\n AllocationBufferSlot ERROR: Memory full.\n");
    return ret;
}

void kfree(const void *objp) {
    if (MemoryCache::freeBufferSlot(objp)) {
        //printString("\nDeallocationBufferSlot ERROR: Invalid address: ");
        //printInt((size_t) objp);
        //printString("\n");
    }
}

int kmem_cache_error(kmem_cache_t *cachep) {
    switch (cachep->cacheError) {
        case 0: printString("\nNo ERROR.\n"); break;
        case 1: printString("\nAllocationCacheSlot ERROR: Memory full.\n"); break;
        case 2: printString("\nDeallocationCacheSlot ERROR: Invalid address.\n"); break;
        case 3: printString("\nAllocationBufferSlot ERROR: Memory full.\n"); break;
    }
    return cachep->cacheError;
}