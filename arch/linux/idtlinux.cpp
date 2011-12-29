#include "idtlinux.h"

IDTLinux::IDTLinux()
{
}

void IDTLinux::initISR()
{
}

void IDTLinux::initIRQ()
{
}

void IDTLinux::installRoutine(unsigned int i, idt_routine_t r)
{
	(void)i;
	(void)r;
}

void IDTLinux::uninstallRoutine(unsigned int i)
{
	(void)i;
}

void IDTLinux::installHandler(unsigned int i, idt_handler_t high, idt_handler_t bottom, void *data)
{
	(void)i;
	(void)high;
	(void)bottom;
	(void)data;
}


idt_routine_t IDTLinux::getRoutine(unsigned int i)
{
	(void)i;
	return NULL;
}
