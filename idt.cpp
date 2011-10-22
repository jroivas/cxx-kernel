#include "idt.h"
#include "types.h"
#include "arch/platform.h"

static IDT *__global_idt = NULL;;

IDT *IDT::get()
{
	if (__global_idt==NULL) {
		__global_idt = Platform::idt();
	}
	return __global_idt;
}

routine_t IDT::routine(unsigned int i)
{
	return get()->getRoutine(i);
}

