#include "kb.h"
#include "types.h"
#include "idt.h"
#include "video.h"
#include "arch/platform.h"

static KB *__global_kb = nullptr;

KB *KB::get()
{
    if (__global_kb == nullptr) {
        __global_kb = Platform::kb();
    }
    return __global_kb;
}

int KB::handler(Regs *r)
{
    if (__global_kb == nullptr) {
        return -1;
    }

    __global_kb->run(r);
    return 0;
}

bool KB::hasKey()
{
    return false;
}


const char *KB::getKey()
{
    return nullptr;
}
