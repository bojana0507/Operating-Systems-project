//
// Created by os on 8/17/22.
//

#ifndef PROJECT_BASE_V1_1_KSEMAPHORE_HPP
#define PROJECT_BASE_V1_1_KSEMAPHORE_HPP

#include "kThread.hpp"

class KSemaphore {

public:

    static MemoryCache* cache;

    void setArg(uint64 init) {
        this->val = init;
        this->on = true;
    }

    ~KSemaphore();

    void operator delete(void* sem) {
        int retValue = cache->freeSlot(sem);
        if (retValue) printStringCons("\nDeallocation error KSemaphore. \n");
    }

    int wait();
    int signal();
    int turnOff();

protected:
    void block();
    void unblock();

private:
    int val;
    bool on;
    static KThread* blockedThreadQueue;

    KThread* get();
    void put(KThread* thread);

};


#endif //PROJECT_BASE_V1_1_KSEMAPHORE_HPP
