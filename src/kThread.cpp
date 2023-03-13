//
// Created by os on 8/10/22.
//

#include "../h/kThread.hpp"
#include "../h/ABI.hpp"
#include "../h/printing.hpp"
#include "../h/kConsole.hpp"

KThread *KThread::running;
uint64 KThread::runningTimeLeft = DEFAULT_TIME_SLICE;
MemoryCache* KThread::cache = nullptr;

void KThread::dispatch() {
    KThread *old = running;
    if (!old->isFinished()) { Scheduler::instance().putReady(old); }

    running = Scheduler::instance().getReady();
    runningTimeLeft = DEFAULT_TIME_SLICE;

    KThread::contextSwitch(&old->context, &running->context);
}

void KThread::threadWrapper() {
    if (running->startRoutine != &KConsole::toControler) syscall_to_user_mode();

    running->startRoutine(running->arg);
    running->setFinished(true);
    syscall_thread_dispatch();
}