//
// Created by os on 8/17/22.
//

#include "../h/kSemaphore.hpp"

MemoryCache* KSemaphore::cache = nullptr;
KThread* KSemaphore::blockedThreadQueue = nullptr;

int KSemaphore::wait() {
    if (!on) return -1;

    if (--val < 0) {
        block();
        if (!on) return -1;
    }
    return 0;
}

int KSemaphore::signal() {
    if (!on) return -1;

    if (++val <= 0) {
        unblock();
    }
    return  0;
}

void KSemaphore::block() {
    KThread *old = KThread::running;
    put(old);
    KThread::running = Scheduler::instance().getReady();
    KThread::contextSwitch(&old->context, &KThread::running->context);
}

void KSemaphore::unblock() {
    Scheduler::instance().putReady(get());
}

KThread *KSemaphore::get() {

    KThread* newThread = blockedThreadQueue;
    if (newThread) blockedThreadQueue = newThread->getNext();

    return newThread;
}

void KSemaphore::put(KThread *thread) {

    KThread* prev = nullptr, *curr = blockedThreadQueue;
    while (curr) {
        prev = curr;
        curr = curr->getNext();
    }
    thread->putNext(curr);
    if (prev) prev->putNext(thread);
    else blockedThreadQueue = thread;

    return;
}

KSemaphore::~KSemaphore() {
    turnOff();
}

int KSemaphore::turnOff() {
    if (!on) return -1;

    KThread* thread = get();
    while (thread) {
        Scheduler::instance().putReady(thread);
        thread = get();
    }
    on = false;
    return 0;
}
