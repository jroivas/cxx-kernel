#ifndef BIOS_H
#define BIOS_H

#include "types.h"
#include "regs.h"
#include "mutex.h"
//#define MM_SLEEP (1<<0) //XXX

class BIOS
{
public:
    static BIOS *get();
    void runInt(uint32_t interrupt, Regs *regs);
    void *alloc(uint32_t size);

private:
    BIOS();
    void setupX86EMU(void *ptr);
    void *bios_pages;
    ptr_val_t free_base;
    Mutex m_bios;
    void *bios_stack;
    void *bios_halt;
};

#endif
