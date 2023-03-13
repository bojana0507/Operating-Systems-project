//
// Created by os on 8/21/22.
//

#ifndef PROJECT_BASE_V1_1_KBUFFER_HPP
#define PROJECT_BASE_V1_1_KBUFFER_HPP

#include "kSemaphore.hpp"

class KBuffer {
    private:
        int cap;
        int *buffer;
        int head, tail;

        KSemaphore* spaceAvailable;
        KSemaphore* itemAvailable;
        KSemaphore* mutexHead;
        KSemaphore* mutexTail;

    public:

    static MemoryCache* cache;

    void initKBuffer(int cap);

    void operator delete(void* thread);

    ~KBuffer();

    void put(int val);
    int get();

    bool isFull();
    bool isEmpty();

    void flush();

};


#endif //PROJECT_BASE_V1_1_KBUFFER_HPP
