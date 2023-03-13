//
// Created by os on 8/21/22.
//

#ifndef PROJECT_BASE_V1_1_KCONSOLE_HPP
#define PROJECT_BASE_V1_1_KCONSOLE_HPP

#include "kBuffer.hpp"
#include "../h/syscall_c.hpp"

class KConsole {

    KConsole() {
        input = (KBuffer*) kmem_cache_alloc(KBuffer::cache);
        input->initKBuffer(1024);
        output = (KBuffer*) kmem_cache_alloc(KBuffer::cache);
        output->initKBuffer(1024);
    }

public:

    KConsole(const KConsole &) = delete;
    KConsole & operator = (const KConsole &) = delete;

    static KConsole& instance(){
        static KConsole singleton;
        return singleton;
    }

    void flush() {
        output->flush();
        delete output;
        delete input;
    }

    void _putc(int c) {
        output->put(c);
    }

    static void toControler(void* arg);

    int fromControler(int c) {
        if (input->isFull()) return -1;
        input->put(c);
        return 0;
    }

    int _getc() {
        return input->get();
    }

private:
    static KBuffer* input;
    static KBuffer* output;

};


#endif //PROJECT_BASE_V1_1_KCONSOLE_HPP
