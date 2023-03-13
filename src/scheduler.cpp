//
// Created by os on 8/12/22.
//

#include "../h/scheduler.hpp"
#include "../h/kThread.hpp"

KThread* Scheduler::readyThreadQueue = nullptr;
KThread* Scheduler::sleepingThreadQueue = nullptr;

KThread *Scheduler::getReady() {

    KThread* newThread = readyThreadQueue;
    if (newThread) readyThreadQueue = readyThreadQueue->getNext();
    return newThread;

}

void Scheduler::putReady(KThread *thread){

    KThread* prev = nullptr, *curr = readyThreadQueue;
    while (curr) {
        prev = curr;
        curr = curr->getNext();
    }
    thread->putNext(curr);
    if (prev) prev->putNext(thread);
    else readyThreadQueue = thread;

    return;
}

bool Scheduler::inReadyQueue(KThread *thread) {

    for (KThread* curr = readyThreadQueue; curr; curr = curr->getNext())
        if (curr == thread) return true;
    return false;

}

bool Scheduler::toAwake() {

    if (!sleepingThreadQueue) return false;
    if (sleepingThreadQueue->getSleepingTime()) return false;
    else return true;

}

void Scheduler::putSleeping(KThread *thread, time_t time) {

    if (!sleepingThreadQueue) {
        //not one is asleep in this moment
        sleepingThreadQueue = thread;
        thread->putNext(nullptr);
        thread->setSleepingTime(time);
    }
    else {
        time_t timeSum = 0;
        KThread* prev = nullptr, *curr = sleepingThreadQueue;
        while (curr->getSleepingTime() + timeSum <= time) {
            timeSum += curr->getSleepingTime();
            prev = curr;
            curr = curr->getNext();
            if (!curr) break;
        }
        thread->setSleepingTime(time - timeSum);
        thread->putNext(curr);
        if (curr) curr->setSleepingTime(curr->getSleepingTime() - thread->getSleepingTime()); //update next time
        if (prev) prev->putNext(thread);
        else {
            sleepingThreadQueue = thread;
        }
    }
    KThread::running = getReady();
    KThread::runningTimeLeft = DEFAULT_TIME_SLICE;
    KThread::contextSwitch(&thread->context, &KThread::running->context);

}

KThread *Scheduler::getSleeping() {

    KThread* thread = sleepingThreadQueue;
    if (thread) sleepingThreadQueue = sleepingThreadQueue->getNext();
    return thread;

}

void Scheduler::decSleepingTime() {

    if (sleepingThreadQueue)
        sleepingThreadQueue->setSleepingTime(sleepingThreadQueue->getSleepingTime() - 1);

}
