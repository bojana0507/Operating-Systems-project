//
// Created by os on 8/21/22.
//

#include "../h/kConsole.hpp"

KBuffer* KConsole::input;
KBuffer* KConsole::output;

void KConsole::toControler(void *arg) {
    while (true) {
        while (*(uint8 *) CONSOLE_STATUS & CONSOLE_TX_STATUS_BIT) {
            *(uint8 *) CONSOLE_TX_DATA = output->get();
        }
        thread_dispatch();
    }

}
