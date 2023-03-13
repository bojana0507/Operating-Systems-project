//
// Created by os on 5/23/22.
//

#ifndef PROJECT_BASE_V1_1_ABI_HPP
#define PROJECT_BASE_V1_1_ABI_HPP
#include "../lib/hw.h"
#include "../h/syscall_c.hpp"

void* syscall_mem_alloc(size_t size);

int syscall_mem_free(void* block);

int syscall_thread_create (
        thread_t* handle,
        void(*startRoutine)(void*),
        void* arg,
        void* stack_space
);

void syscall_thread_dispatch();

int syscall_thread_exit();

int syscall_thread_create_notstart (
        thread_t* handle,
        void(*startRoutine)(void*),
        void* arg,
        void* stack_space
);

void syscall_delete_thread(thread_t thread);

int syscall_thread_start(thread_t thread);

void syscall_to_user_mode();

void syscall_putc(char c);

char syscall_getc();

int syscall_sem_open(sem_t* handle, unsigned init);

int syscall_sem_close(sem_t handle);

int syscall_sem_wait(sem_t id);

int syscall_sem_signal(sem_t id);

void syscall_delete_sem(sem_t myHandle);

void syscall_time_sleep(time_t time);

void syscall_set_period(thread_t myHandle, time_t period);

time_t syscall_get_period(thread_t myHandle);

#endif //PROJECT_BASE_V1_1_ABI_HPP
