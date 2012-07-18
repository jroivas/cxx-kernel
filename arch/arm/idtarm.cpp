#include "idtarm.h"

IDTARM::IDTARM()
{
}
void print_uart0(const char *s);

void IDTARM::initISR()
{
}

void IDTARM::initIRQ()
{
}

void IDTARM::installRoutine(unsigned int i, idt_routine_t r)
{
	(void)i;
	(void)r;
}

void IDTARM::uninstallRoutine(unsigned int i)
{
	(void)i;
}

void IDTARM::installHandler(unsigned int i, idt_handler_t high, idt_handler_t bottom, void *data)
{
	(void)i;
	(void)high;
	(void)bottom;
	(void)data;
}


idt_routine_t IDTARM::getRoutine(unsigned int i)
{
	(void)i;
	return NULL;
}
