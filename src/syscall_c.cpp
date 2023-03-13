//
// Created by os on 5/23/22.
//

#include "../h/syscall_c.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/ABI.hpp"
#include "../h/printing.hpp"

void* mem_alloc(size_t size){
    void* allocatedAddr = syscall_mem_alloc(MemoryAllocator::toBlocks(size));
    return allocatedAddr;
}

int mem_free(void* block){
    return syscall_mem_free(block);
}

int thread_create (
        thread_t* handle,
        void(*startRoutine)(void*),
        void* arg
    ){
    void* stack = nullptr;
    if (startRoutine) {
        stack = mem_alloc(DEFAULT_STACK_SIZE*sizeof(uint64));
        if (!stack) {
            printString("\nNo heap space for stack! \n");
            return -1;
        }
    }
    uint64 retValue = syscall_thread_create(
        handle,
        startRoutine,
        arg,
        stack
    );
    if (retValue) {
        printString("\nNo heap space for thread! \n");
        syscall_mem_free(stack);
        return -2;
    }
    return 0;
}

void thread_dispatch (){
    syscall_thread_dispatch();
}

int thread_exit (){
    int retValue = syscall_thread_exit();
    if (retValue)
        return retValue;
    thread_dispatch();
    return retValue;
}

int sem_open (
        sem_t* handle,
        unsigned init
    ) {
    return syscall_sem_open(handle, init);
}

int sem_close (sem_t handle) {
    return syscall_sem_close(handle);
}

int sem_wait (sem_t id) {
    return syscall_sem_wait(id);
}

int sem_signal (sem_t id) {
    return syscall_sem_signal(id);
}

int time_sleep (time_t time) {
    if (time == 0) return -1;
    syscall_time_sleep(time);
    return 0;
}

void putc(char c) {
    syscall_putc(c);
}

char getc() {
    return syscall_getc();
}