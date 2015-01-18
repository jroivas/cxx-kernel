#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "regs.h"

#define SYSTEM_CALL_INTERRUPT 0x99

class SysCall
{
public:
    SysCall();
    static int handler(Regs *r);

protected:
};

#endif
