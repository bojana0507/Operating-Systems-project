//
// Created by os on 5/23/22.
//

#include "../h/syscall_cpp.hpp"
#include "../h/printing.hpp"
#include "../h/ABI.hpp"

using size_tt = decltype(sizeof(0));

void* operator new(size_tt size) {
    return mem_alloc(size);
}

void* operator new[](size_tt size) {
    return mem_alloc(size);
}

void operator delete(void* ptr) {
    if (mem_free(ptr))
        printString("\nDeallocate error CPP API! \n");
}

void operator delete[](void* ptr) {
    if (mem_free(ptr))
        printString("\nDeallocate[] error CPP API! \n");
}

class HandleGetter {
    friend PeriodicThread;
private:
    static thread_t getThreadHandle(PeriodicThread *thread) {
        return thread->myHandle;;
    }
};

Thread::Thread (void (*body)(void*), void* arg){
    void* stack = mem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64));

    if (!stack) {
        printString("\nAllocate stack error CPP API! \n");
    }

    uint64 retValue = syscall_thread_create_notstart(
            &myHandle,
            body,
            arg,
            stack
    );
    if (retValue) {
        printString("\nAllocate thread error CPP API! \n");
        syscall_mem_free(stack);
    }

}

Thread::~Thread() {
    syscall_delete_thread(myHandle);
}

int Thread::start() {
   return syscall_thread_start(myHandle);
}

void Thread::dispatch() {
    thread_dispatch();
}

Thread::Thread() {

    void* stack = mem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64));
    if (!stack) {
        //printString("\nAllocate stack error CPP API Thread()! \n");
    }

    uint64 retValue = syscall_thread_create_notstart(
            &myHandle,
            &runWrapper,
            this,
            stack
    );
    if (retValue) {
        printString("\nAllocate Thread() error CPP API! \n");
        syscall_mem_free(stack);
    }

}

void Thread::runWrapper(void *thread) {
    ((Thread*)thread)->run();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Semaphore::Semaphore(unsigned int init) {
    if (sem_open(&myHandle, init))
        printString("\nAllocate Semaphore error CPP API! \n");
}

Semaphore::~Semaphore() {
    syscall_delete_sem(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

PeriodicThread::PeriodicThread(time_t period) {
    syscall_set_period(HandleGetter::getThreadHandle(this), period);
}

void PeriodicThread::run() {
    while (true) {
        periodicActivation();
        time_sleep(syscall_get_period(HandleGetter::getThreadHandle(this)));
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
