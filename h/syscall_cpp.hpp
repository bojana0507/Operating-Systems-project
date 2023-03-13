//
// Created by os on 5/23/22.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_CPP_HPP
#define PROJECT_BASE_V1_1_SYSCALL_CPP_HPP

#include "syscall_c.hpp"

using size_tt = decltype(sizeof(0));

void* operator new (size_tt);
void operator delete (void*);

void* operator new[](size_tt size);
void operator delete[](void* ptr);

class HandleGetter;

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);
protected:
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
    static void runWrapper(void* thread);
    friend HandleGetter;
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
    void run ();
};

class Console {
public:
    static char getc ();
    static void putc (char);
};

#endif //PROJECT_BASE_V1_1_SYSCALL_CPP_HPP