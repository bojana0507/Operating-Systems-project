//
// Created by os on 8/10/22.
//

#ifndef PROJECT_BASE_V1_1_KTHREAD_HPP
#define PROJECT_BASE_V1_1_KTHREAD_HPP

#include "scheduler.hpp"
#include "MemoryAllocator.hpp"
#include "slab.hpp"
#include "riscv.hpp"
#include "print.hpp"
#include "kSemaphore.hpp"

typedef KThread* thread_t;
typedef void (*Body)(void*);

class KThread {

    friend Riscv;
    friend class KSemaphore;
    friend Scheduler;

public:
    ~KThread() {
        int retValue = 0;
        if (stack) retValue = MemoryAllocator::instance().free(stack);
        if (retValue) printStringCons("\nDeallocation error stack. \n");
    }

    void operator delete(void* thread) {
        kmem_cache_free(cache, thread);
    }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    static bool isMain() {
        if (running->stack == nullptr) return true;
        else return false;
    }

    void setArgs(Body startRoutine,
                 void* arg,
                 uint64* stack){
        this->startRoutine = startRoutine;
        this->arg = arg;
        this->stack = startRoutine != nullptr ? stack : nullptr;
        this->context = {(uint64)threadWrapper,
                 stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                };
        this->finished = false;
        this->sleepTime = 0;
    }

    void addToReadyQueue() {
        if (startRoutine != nullptr) {
            Scheduler::instance().putReady(this);
        }
    }

    void setSleepingTime(uint64 time) { sleepTime = time; }

    uint64 getSleepingTime() { return sleepTime; }

    void setPeriodTime(uint64 time) { periodTime = time; }

    uint64 getPeriodTime() { return periodTime; }

    void putNext(KThread* thread) { next = thread; }

    KThread* getNext() { return next; }

    static void dispatch();

    static KThread *running;
    static uint64 runningTimeLeft;
    static MemoryCache* cache;

private:

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body startRoutine;
    uint64 *stack;
    void* arg;
    Context context;
    bool finished;
    KThread* next;
    uint64 sleepTime;
    uint64 periodTime;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void threadWrapper();

};


#endif //PROJECT_BASE_V1_1_KTHREAD_HPP
