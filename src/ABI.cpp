//
// Created by os on 5/23/22.
//

#include "../h/ABI.hpp"
#include "../h/printing.hpp"

void* syscall_mem_alloc(size_t size) {
    asm volatile("mv a1, %0" : : "r" (size));
    asm volatile("mv a0, %0" : : "r" (1));
    __asm__ volatile ( "ecall" );
    void* volatile allocatedAddr;
    asm volatile("mv %0, a0" : "=r" (allocatedAddr));
    return allocatedAddr;
}

int syscall_mem_free(void* block) {
    asm volatile("mv a1, %0" : : "r" (block));
    asm volatile("mv a0, %0" : : "r" (2));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

int syscall_thread_create (
        thread_t* handle,
        void(*startRoutine)(void*),
        void* arg,
        void* stack
    ){
    asm volatile("mv a4, %0" : : "r" (stack));
    asm volatile("mv a3, %0" : : "r" (arg));
    asm volatile("mv a2, %0" : : "r" (startRoutine));
    asm volatile("mv a1, %0" : : "r" (handle));
    asm volatile("mv a0, %0" : : "r" (17));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

void syscall_thread_dispatch() {
    asm volatile("mv a0, %0" : : "r" (19));
    __asm__ volatile ( "ecall" );
}

int syscall_thread_exit() {
    asm volatile("mv a0, %0" : : "r" (18));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

int syscall_thread_create_notstart(
        thread_t* handle,
        void(*startRoutine)(void*),
        void* arg,
        void* stack
    ){
        asm volatile("mv a4, %0" : : "r" (stack));
        asm volatile("mv a3, %0" : : "r" (arg));
        asm volatile("mv a2, %0" : : "r" (startRoutine));
        asm volatile("mv a1, %0" : : "r" (handle));
        asm volatile("mv a0, %0" : : "r" (16));
        __asm__ volatile ( "ecall" );
        uint64 volatile retValue;
        asm volatile("mv %0, a0" : "=r" (retValue));
        return retValue;
}

void syscall_delete_thread(thread_t thread) {
    asm volatile("mv a1, %0" : : "r" (thread));
    asm volatile("mv a0, %0" : : "r" (15));
    __asm__ volatile ( "ecall" );
}

int syscall_thread_start(thread_t thread) {
    asm volatile("mv a1, %0" : : "r" (thread));
    asm volatile("mv a0, %0" : : "r" (14));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

void syscall_to_user_mode() {
    asm volatile("mv a0, %0" : : "r" (3));
    __asm__ volatile ( "ecall" );
}

int syscall_sem_open(sem_t* handle, unsigned init) {
    asm volatile("mv a2, %0" : : "r" (init));
    asm volatile("mv a1, %0" : : "r" (handle));
    asm volatile("mv a0, %0" : : "r" (33));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

int syscall_sem_close(sem_t handle) {
    asm volatile("mv a1, %0" : : "r" (handle));
    asm volatile("mv a0, %0" : : "r" (34));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

int syscall_sem_wait(sem_t id) {
    asm volatile("mv a1, %0" : : "r" (id));
    asm volatile("mv a0, %0" : : "r" (35));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

int syscall_sem_signal(sem_t id) {
    asm volatile("mv a1, %0" : : "r" (id));
    asm volatile("mv a0, %0" : : "r" (36));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

void syscall_delete_sem(sem_t myHandle) {
    asm volatile("mv a1, %0" : : "r" (myHandle));
    asm volatile("mv a0, %0" : : "r" (37));
    __asm__ volatile ( "ecall" );
}

char syscall_getc() {
    asm volatile("mv a0, %0" : : "r" (65));
    __asm__ volatile ( "ecall" );
    uint64 retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}

void syscall_putc(char c) {
    asm volatile("mv a1, %0" : : "r" (c));
    asm volatile("mv a0, %0" : : "r" (66));
    __asm__ volatile ( "ecall" );
}

void syscall_time_sleep(time_t time) {
    asm volatile("mv a1, %0" : : "r" (time));
    asm volatile("mv a0, %0" : : "r" (49));
    __asm__ volatile ( "ecall" );
}

void syscall_set_period(thread_t thread, time_t period) {
    asm volatile("mv a2, %0" : : "r" (period));
    asm volatile("mv a1, %0" : : "r" (thread));
    asm volatile("mv a0, %0" : : "r" (50));
    __asm__ volatile ( "ecall" );
}

time_t syscall_get_period(thread_t thread) {
    asm volatile("mv a1, %0" : : "r" (thread));
    asm volatile("mv a0, %0" : : "r" (51));
    __asm__ volatile ( "ecall" );
    uint64 volatile retValue;
    asm volatile("mv %0, a0" : "=r" (retValue));
    return retValue;
}