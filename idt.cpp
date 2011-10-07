#include "idt.h"
#include "regs.h"
#include "port.h"
#include "types.h"
#include "string.h"

//extern "C" void idt_load(void *p);
extern "C" void idt_load(IDT::Ptr p);

IDT::IDT()
{
	idtp.limit = (sizeof(Entry)*256)-1;
	idtp.base = (unsigned int)&idt;

	Mem::set(&idt, 0, sizeof(Entry)*256);
	load();
}

void IDT::load()
{
	idt_load(idtp);
	//idt_load(&idtp);
/*
	asm volatile ( "lidt [%0]"
			: 
			: "r" (idtp));
*/
}

void IDT::initISR()
{
}

void IDT::initIRQ()
{
}

extern "C" void irq_handler(Regs * r)
{
	void (*handler)(Regs *r);
	handler = NULL;

//	handler = irq_routines[r->int_no - 32];
	if (handler) {
		handler(r);
	}

	if (r->int_no >= 40) {
		Port::out(0xA0, 0x20);
	}

	Port::out(0x20, 0x20);
}
