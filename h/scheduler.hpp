//
// Created by os on 8/10/22.
//

#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP

#include "../lib/hw.h"

class KThread;

class Scheduler
{
private:

    static KThread* readyThreadQueue;
    static KThread* sleepingThreadQueue;

public:

    Scheduler() {}
    Scheduler(const Scheduler &) = delete;
    Scheduler & operator = (const Scheduler &) = delete;

    static Scheduler& instance(){
        static Scheduler singleton;
        return singleton;
    }

    KThread *getReady();

    void putReady(KThread *thread);

    void decSleepingTime();

    bool toAwake();

    void putSleeping(KThread *thread, time_t time);

    KThread* getSleeping();

    bool inReadyQueue(KThread* thread);

};

#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP
