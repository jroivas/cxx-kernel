#include "kb.h"
#include "types.h"
#include "idt.h"
#include "video.h"
#include "arch/platform.h"

static KB *__global_kb = NULL;

KB *KB::get()
{
    if (__global_kb==NULL) {
        __global_kb = Platform::kb();
    }
    return __global_kb;
}

int KB::handler(Regs *r)
{
    if (__global_kb == NULL) {
        return -1;
    }

    __global_kb->run(r);
    return 0;
}
