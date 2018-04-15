#include "idt.h"
#include "types.h"
#include "arch/platform.h"

static IDT *__global_idt = nullptr;

IDT *IDT::get()
{
    if (__global_idt == nullptr) {
        __global_idt = Platform::idt();
    }
    return __global_idt;
}

idt_routine_t IDT::routine(unsigned int i)
{
    if (get() == nullptr) return nullptr;
    return get()->getRoutine(i);
}
