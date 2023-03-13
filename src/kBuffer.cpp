//
// Created by os on 8/21/22.
//

#include "../h/kBuffer.hpp"


MemoryCache* KBuffer::cache = nullptr;

void KBuffer::initKBuffer(int capReal) {
    cap = capReal + 1;
    head = tail = 0;
    buffer = (int *)kmalloc(sizeof(int) * cap);
    itemAvailable = (KSemaphore*) kmem_cache_alloc(KSemaphore::cache);
    itemAvailable->setArg(0);

    spaceAvailable = (KSemaphore*) kmem_cache_alloc(KSemaphore::cache);
    spaceAvailable->setArg(capReal);

    mutexHead = (KSemaphore*) kmem_cache_alloc(KSemaphore::cache);
    mutexHead->setArg(1);

    mutexTail = (KSemaphore*) kmem_cache_alloc(KSemaphore::cache);
    mutexTail->setArg(1);

}

KBuffer::~KBuffer() {
    kfree(buffer);
    delete itemAvailable;
    delete spaceAvailable;
    delete mutexTail;
    delete mutexHead;
}

void KBuffer::put(int val) {
    spaceAvailable->wait();

    mutexTail->wait();
    buffer[tail] = val;
    tail = (tail + 1) % cap;
    mutexTail->signal();

    itemAvailable->signal();
}

int KBuffer::get() {

    itemAvailable->wait();

    mutexHead->wait();
    int ret = buffer[head];
    head = (head + 1) % cap;
    mutexHead->signal();

    spaceAvailable->signal();

    return ret;
}

bool KBuffer::isFull() {
    if (head == ((tail + 1) % cap)) return true;
    else return false;
}

bool KBuffer::isEmpty() {
    if (head == tail) return true;
    else return false;
}

void KBuffer::flush() {
    while (*(uint8 *) CONSOLE_STATUS & CONSOLE_TX_STATUS_BIT) {
        if (isEmpty()) break;
        *(uint8 *) CONSOLE_TX_DATA = buffer[head];
        head = (head + 1) % cap;
    }
}

void KBuffer::operator delete(void *kBuffer) {
    kmem_cache_free(cache, kBuffer);
}


