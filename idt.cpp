#include "idt.h"
#include "port.h"
#include "types.h"
#include "string.h"

extern "C" void idt_load(void *p);
//extern "C" void idt_load(IDT::Ptr p);

static IDT *global_idt = NULL;;

IDT *IDT::getInstance()
{
	if (global_idt==NULL) {
		global_idt = new IDT();
	}
	return global_idt;
}

IDT::IDT()
{
	idtp.limit = (sizeof(Entry)*256)-1;
	idtp.base = (unsigned int)&idt;

	Mem::set(&idt, 0, sizeof(Entry)*256);
	load();

	for (int i=0; i<IRQ_ROUTINES; i++) {
		routines[i] = 0;
	}
}

void IDT::load()
{
	idt_load(&idtp);
	//idt_load(&idtp);
}

void IDT::installRoutine(unsigned int i, void (*handler)(Regs *r))
{
	routines[i] = handler;
}

routine_t IDT::routine(unsigned int i)
{
	return getInstance()->routines[i];
	//return NULL;
}

void IDT::setGate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
	idt[num].base_low = (base & 0xFFFF);
	idt[num].base_high = ((base>>16) & 0xFFFF);

	idt[num].sel = sel;
	idt[num].flags = flags;
	idt[num].always0 = 0;
}

#define ISR_GATE_DEF(x) extern "C" void isr##x();
#define ISR_GATE(x) setGate(x, (unsigned)isr##x, 0x08, 0x8E);
ISR_GATE_DEF(0);
ISR_GATE_DEF(1);
ISR_GATE_DEF(2);
ISR_GATE_DEF(3);
ISR_GATE_DEF(4);
ISR_GATE_DEF(5);
ISR_GATE_DEF(6);
ISR_GATE_DEF(7);
ISR_GATE_DEF(8);
ISR_GATE_DEF(9);
ISR_GATE_DEF(10);
ISR_GATE_DEF(11);
ISR_GATE_DEF(12);
ISR_GATE_DEF(13);
ISR_GATE_DEF(14);
ISR_GATE_DEF(15);
ISR_GATE_DEF(16);
ISR_GATE_DEF(17);
ISR_GATE_DEF(18);
ISR_GATE_DEF(19);
ISR_GATE_DEF(20);
ISR_GATE_DEF(21);
ISR_GATE_DEF(22);
ISR_GATE_DEF(23);
ISR_GATE_DEF(24);
ISR_GATE_DEF(25);
ISR_GATE_DEF(26);
ISR_GATE_DEF(27);
ISR_GATE_DEF(28);
ISR_GATE_DEF(29);
ISR_GATE_DEF(30);
ISR_GATE_DEF(31);
void IDT::initISR()
{
	ISR_GATE(0);
	ISR_GATE(1);
	ISR_GATE(2);
	ISR_GATE(3);
	ISR_GATE(4);
	ISR_GATE(5);
	ISR_GATE(6);
	ISR_GATE(7);
	ISR_GATE(8);
	ISR_GATE(9);
	ISR_GATE(10);
	ISR_GATE(11);
	ISR_GATE(12);
	ISR_GATE(13);
	ISR_GATE(14);
	ISR_GATE(15);
	ISR_GATE(16);
	ISR_GATE(17);
	ISR_GATE(18);
	ISR_GATE(19);
	ISR_GATE(20);
	ISR_GATE(21);
	ISR_GATE(22);
	ISR_GATE(23);
	ISR_GATE(24);
	ISR_GATE(25);
	ISR_GATE(26);
	ISR_GATE(27);
	ISR_GATE(28);
	ISR_GATE(29);
	ISR_GATE(30);
	ISR_GATE(31);
}

#define IRQ_GATE_DEF(x) extern "C" void irq##x();
#define IRQ_GATE(x) setGate(x+32, (unsigned)irq##x, 0x08, 0x8E);
IRQ_GATE_DEF(0);
IRQ_GATE_DEF(1);
IRQ_GATE_DEF(2);
IRQ_GATE_DEF(3);
IRQ_GATE_DEF(4);
IRQ_GATE_DEF(5);
IRQ_GATE_DEF(6);
IRQ_GATE_DEF(7);
IRQ_GATE_DEF(8);
IRQ_GATE_DEF(9);
IRQ_GATE_DEF(10);
IRQ_GATE_DEF(11);
IRQ_GATE_DEF(12);
IRQ_GATE_DEF(13);
IRQ_GATE_DEF(14);
IRQ_GATE_DEF(15);
void IDT::initIRQ()
{
	IRQ_GATE(0);
	IRQ_GATE(1);
	IRQ_GATE(2);
	IRQ_GATE(3);
	IRQ_GATE(4);
	IRQ_GATE(5);
	IRQ_GATE(6);
	IRQ_GATE(7);
	IRQ_GATE(8);
	IRQ_GATE(9);
	IRQ_GATE(10);
	IRQ_GATE(11);
	IRQ_GATE(12);
	IRQ_GATE(13);
	IRQ_GATE(14);
	IRQ_GATE(15);
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

extern "C" void isr_handler(Regs * r)
{
	if (r->int_no < 32) {
		// Got it
	}
}
