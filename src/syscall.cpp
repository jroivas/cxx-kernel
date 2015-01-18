#include "syscall.h"
#include "platform.h"

SysCall::SysCall()
{
    IDT::get()->installRoutine(SYSTEM_CALL_INTERRUPT, &SysCall::handler);
}

int SysCall::handler(Regs *r)
{
    // FIXME
    (void)r;
    Platform::video()->printf("System call!\n");
    return 0x42;
}
