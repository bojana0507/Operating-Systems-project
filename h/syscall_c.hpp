//
// Created by os on 5/23/22.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_HPP
#define PROJECT_BASE_V1_1_SYSCALL_C_HPP
#include "../lib/hw.h"

void* mem_alloc(size_t size);

int mem_free(void* block);

class KThread;
typedef KThread* thread_t;
int thread_create (
        thread_t* handle,
        void(*startRoutine)(void*),
        void* arg
);

void thread_dispatch ();

int thread_exit ();

class KSemaphore;
typedef KSemaphore* sem_t;
int sem_open (
        sem_t* handle,
        unsigned init
);

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);

typedef unsigned long time_t;
int time_sleep (time_t);

void putc(char c);

const int EOF = -1;
char getc();

#endif //PROJECT_BASE_V1_1_SYSCALL_C_HPP