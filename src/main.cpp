//
// Created by os on 6/20/22.
//
//CXXFLAGS += -fcheck-new

#include "../h/riscv.hpp"
#include "../h/printing.hpp"
#include "../h/slab.hpp"
#include "../h/kThread.hpp"
#include "../h/kConsole.hpp"

extern void userMain();

extern void buddyPrint();

void userMainWrapper(void* arg){
    userMain();
}

int main() {
    Riscv::w_stvec((uint64)&Riscv::interruptRoutine);

    uint64 retValue = Riscv::r_sie();
    retValue &= ~(Riscv::BitMaskSie::SIE_SEIE | Riscv::BitMaskSie::SIE_SSIE);
    Riscv::w_sie(retValue);

    Scheduler::instance();
    kmem_init(nullptr,0);

    KThread::cache = kmem_cache_create("KThread", sizeof(KThread), nullptr, nullptr);
    KSemaphore::cache = kmem_cache_create("KSemaphore", sizeof(KSemaphore), nullptr, nullptr);
    KBuffer::cache = kmem_cache_create("KBuffer", sizeof(KBuffer), nullptr, nullptr);
    KConsole::instance();

    KThread* mainthread, *userthread, *consolethread;

    int retmain = thread_create (
            &mainthread,
            nullptr,
            nullptr
    );

    if(retmain)
        printString("\nError allocating main thread! \n");

    KThread::running = mainthread;

    int retuser = thread_create (
            &userthread,
            userMainWrapper,
            nullptr
    );

    if(retuser)
        printString("\nError allocating usermain thread! \n");

    int retconsole = thread_create (
            &consolethread,
            KConsole::toControler,
            &userthread
    );

    if(retconsole)
        printString("\nError allocating console thread! \n");

    retValue = Riscv::r_sie();
    retValue |= (Riscv::BitMaskSie::SIE_SEIE | Riscv::BitMaskSie::SIE_SSIE);
    Riscv::w_sie(retValue);

    retValue = Riscv::r_sstatus();
    retValue |= (Riscv::SSTATUS_SIE);
    Riscv::w_sstatus(retValue);

    while (!userthread->isFinished()) {
        /* wait */
        thread_dispatch();
    }

    retValue = Riscv::r_sie();
    retValue &= ~(Riscv::BitMaskSie::SIE_SEIE | Riscv::BitMaskSie::SIE_SSIE);
    Riscv::w_sie(retValue);

    KConsole::instance().flush();
    delete userthread;
    delete consolethread;
    delete mainthread;

    kmem_cache_destroy(KThread::cache);
    kmem_cache_destroy(KSemaphore::cache);
    kmem_cache_destroy(KBuffer::cache);

    return 0;
}