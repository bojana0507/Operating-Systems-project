//
// Created by os on 6/21/22.
//

#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/kConsole.hpp"
#include "../lib/console.h"

void printStringCons(char const *string)
{
    //uint64 sstatus = Riscv::r_sstatus();
    //Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    while (*string != '\0')
    {
        KConsole::instance()._putc(*string);
        string++;
    }
    //Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}

void printIntegerCons(uint64 integer)
{
    //uint64 sstatus = Riscv::r_sstatus();
    //Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    static char digits[] = "0123456789";
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if (integer < 0)
    {
        neg = 1;
        x = -integer;
    } else
    {
        x = integer;
    }

    i = 0;
    do
    {
        buf[i++] = digits[x % 10];
    } while ((x /= 10) != 0);
    if (neg)
        buf[i++] = '-';

    while (--i >= 0) { KConsole::instance()._putc(buf[i]); }
    //Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}