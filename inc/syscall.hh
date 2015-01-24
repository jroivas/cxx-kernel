#ifndef _SYSCALL_HH
#define _SYSCALL_HH

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
