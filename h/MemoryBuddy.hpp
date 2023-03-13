//
// Created by os on 2/3/23.
//

#ifndef PROJECT_BASE_V1_1_MEMORYBUDDY_HPP
#define PROJECT_BASE_V1_1_MEMORYBUDDY_HPP

#include "MemoryAllocator.hpp"
#include "slab.hpp"

class MemoryBuddy {

private:

    MemoryBuddy();

public:

    MemoryBuddy(const MemoryBuddy &) = delete;
    MemoryBuddy & operator = (const MemoryBuddy &) = delete;

    static MemoryBuddy& instance(){
        static MemoryBuddy singleton;
        return singleton;
    }

    static void* alloc(int i);

    static void free(void* segment, int i);

    static size_t* merge(size_t* segment, int i);

    static size_t sizeMax;
    static int expMax;
    static int expMin;
    static size_t sizeMin;
    static size_t** freeLists;


};


#endif //PROJECT_BASE_V1_1_MEMORYBUDDY_HPP
