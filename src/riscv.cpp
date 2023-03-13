//
// Created by os on 6/19/22.
//

#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/kThread.hpp"
#include "../h/ABI.hpp"
#include "../h/print.hpp"
#include "../h/kConsole.hpp"

const uint64 SCAUSE_BTN = ((uint64) 1 << 63);

void Riscv::handleInterruptRoutine() {
    uint64 volatile syscall_num;
    asm volatile("mv %0, a0" : "=r" (syscall_num));
    uint64 volatile sepc = r_sepc();
    uint64 volatile sstatus = r_sstatus();
    uint64 volatile scause = r_scause();

    void volatile *param1;
    void volatile *param2;
    void volatile *param3;
    void volatile *param4;
    asm volatile("mv %0, a1" : "=r" (param1));
    asm volatile("mv %0, a2" : "=r" (param2));
    asm volatile("mv %0, a3" : "=r" (param3));
    asm volatile("mv %0, a4" : "=r" (param4));

    if (SCAUSE_BTN & scause) {
        /*external interrupt*/
        if ((scause & ~SCAUSE_BTN) == 1){
            //timer interrupt

            //sleep update
            Scheduler::instance().decSleepingTime();
            while (Scheduler::instance().toAwake())
                Scheduler::instance().putReady(Scheduler::instance().getSleeping());

            //running update
            KThread::runningTimeLeft--;
            if (KThread::runningTimeLeft <= 0) {
                KThread::dispatch();
                //w_sstatus(sstatus);
                //w_sepc(sepc);
            }
            mc_sip(SIP_SSIP);
        }

        else if ((scause & ~SCAUSE_BTN) == 9){
            //console interrupt
            uint64 numEI = plic_claim();
            if (numEI == 10) {
                while (*(uint8*)CONSOLE_STATUS & CONSOLE_RX_STATUS_BIT) {
                    if (KConsole::instance().fromControler(*(uint8 *) CONSOLE_RX_DATA))
                        break; //if full input buffer - leave for later
                }
            }
            plic_complete(10);
            mc_sip(SIP_SEIP);
        }
    }

    else {
        switch(scause){
            case 2:
                printStringCons("Illegal instruction \n");
                thread_exit();
                KThread::dispatch();
                break;
            case 5:
                printStringCons("Illegal read address \n");
                thread_exit();
                KThread::dispatch();
                break;
            case 7:
                printStringCons("Illegal write address \n");
                thread_exit();
                KThread::dispatch();
                break;
            case 8:
                //Ecall user mode
            case 9:
                //Ecall system mode
                sepc += 4;
                void* allocatedAddr;
                uint64 retValue;
                KThread* thread;
                KSemaphore* sem;
                char c;
                switch (syscall_num) {
                    case 1:
                        //syscall_mem_alloc
                        allocatedAddr = MemoryAllocator::instance().alloc((uint64)param1);;
                        asm volatile("mv a0, %0" : : "r" (allocatedAddr));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 2:
                        //syscall_mem_free
                        retValue = MemoryAllocator::instance().free((void*)param1);
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 3:
                        //syscall_to_user_mode
                        sstatus &= ~BitMaskSstatus::SSTATUS_SPP;
                        sstatus |= BitMaskSstatus::SSTATUS_SPIE;
                        break;

                    case 14:
                        //syscall_thread_start
                        thread = (KThread*)param1;
                        retValue = 0;
                        if (!thread->stack) retValue = -1;
                        else if (Scheduler::instance().inReadyQueue(thread) || thread->isFinished()) retValue = -1;
                        else thread->addToReadyQueue();
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 15:
                        //syscall_delete_thread
                        thread = (KThread*)param1;
                        if (KThread::running == thread) break;
                        delete thread;
                        break;

                    case 16:
                        //syscall_thread_create_notstart
                        thread = (KThread*) kmem_cache_alloc(KThread::cache);
                        retValue = -2;
                        if (thread) {
                            thread->setArgs((Body) param2, (void*)param3, (uint64*)param4);
                            *((thread_t *)param1) = thread;
                            retValue = 0;
                        }
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 17:
                        //syscall_thread_create
                        thread = (KThread*) kmem_cache_alloc(KThread::cache);
                        retValue = -2;
                        if (thread) {
                            thread->setArgs((Body) param2, (void*)param3, (uint64*)param4);
                            //if (param4)
                                thread->addToReadyQueue();
                            *((thread_t *)param1) = thread;
                            retValue = 0;
                        }
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 18:
                        //syscall_thread_exit
                        if (KThread::running->isFinished() || KThread::isMain())
                            retValue = -1;
                        else {
                            KThread::running->setFinished(true);
                            retValue = 0;
                        }
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 19:
                        //syscall_thread_dispatch
                        w_sepc(sepc);
                        KThread::dispatch();
                        break;

                    case 33:
                        //syscall_sem_open
                        sem = (KSemaphore*) kmem_cache_alloc(KSemaphore::cache);
                        retValue = -1;
                        if (sem) {
                            sem->setArg((uint64)param2);
                            *((sem_t*)param1) = sem;
                            retValue = 0;
                        }
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 34:
                        //syscall_sem_close
                        sem = (sem_t) param1;
                        retValue = sem->turnOff();
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 35:
                        //syscall_sem_wait
                        sem = (sem_t) param1;
                        retValue = sem->wait();
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 36:
                        //syscall_sem_signal
                        sem = (sem_t) param1;
                        retValue = sem->signal();
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 37:
                        //syscall_delete_sem
                        sem = (sem_t) param1;
                        delete sem;
                        break;

                    case 49:
                        //syscall_time_sleep
                        Scheduler::instance().putSleeping(KThread::running, (time_t)param1);
                        break;

                    case 50:
                        //syscall_set_period
                        thread = (KThread*) param1;
                        thread->setPeriodTime((uint64) param2);
                        break;

                    case 51:
                        //syscall_get_period
                        thread = (KThread*) param1;
                        retValue = thread->getPeriodTime();
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 65:
                        //syscall_getc
                        retValue = KConsole::instance()._getc();
                        asm volatile("mv a0, %0" : : "r" (retValue));
                        asm volatile("sd a0, 80(fp)");
                        break;

                    case 66:
                        //syscall_putc
                        c = reinterpret_cast<char&>(param1);
                        KConsole::instance()._putc(c);
                        break;

                    default:
                        printStringCons("\nUnexpected syscall num: ");
                        printIntegerCons(syscall_num);
                        printStringCons("!\n");

                }
        }
    }
    w_sepc(sepc);
    w_sstatus(sstatus);
}