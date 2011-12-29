#include "idtx86.h"
#include "port.h"
#include "types.h"
#include "string.h"
//#include "x86.h"
#include "videox86.h"
#include "arch/platform.h"

extern "C" void idt_load();

IDTX86::Ptr idt_idtp;

IDTX86::IDTX86() : IDT()
{
	idt_idtp.limit = (sizeof(Entry)*256)-1;
	idt_idtp.base = (unsigned int)&idt;

	Mem::set(&idt, 0, sizeof(Entry)*256);

	for (int i=0; i<IRQ_ROUTINES; i++) {
		routines[i] = NULL;
	}
	for (int i=0; i<ISR_HANDLERS; i++) {
		handlers[i] = NULL;
	}

	load();
}

void IDTX86::load()
{
	idt_load();
}

//void IDTX86::installRoutine(unsigned int i, void (*handler)(Regs *r))
void IDTX86::installRoutine(unsigned int i, idt_routine_t handler)
{
	routines[i] = handler;
}

void IDTX86::uninstallRoutine(unsigned int i)
{
	routines[i] = NULL;
}

void IDTX86::installHandler(unsigned int i, idt_handler_t high, idt_handler_t bottom, void *data)
{
	Handler *h = new Handler();
	h->num = i;
	h->high_half = high;
	h->bottom_half = bottom;
	h->data = data;

	//TODO: create a thread if necessary

	/* Installing handler */
	if (handlers[i]==NULL) {
		handlers[i] = h;
	} else {
		Handler *p = handlers[i];
		while (p->next!=NULL) {
			p = p->next;
		}
		p->next = h;
	}
}

idt_routine_t IDTX86::getRoutine(unsigned int i)
{
	return routines[i];
}

IDTX86::Handler *IDTX86::getHandler(unsigned int i)
{
	return handlers[i];
}

IDTX86::Handler *IDTX86::handler(unsigned int i)
{
	if (get()==NULL) return NULL;
	return ((IDTX86*)get())->getHandler(i);
}

void IDTX86::setGate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
	idt[num].base_low = (base & 0xFFFF);
	idt[num].base_high = ((base>>16) & 0xFFFF);

	idt[num].sel = sel;
	idt[num].flags = flags;
	idt[num].always0 = 0;
}

#define ISR_GATE_DEF(x) extern "C" void __isr##x();
#define ISR_GATE(x) setGate(x, (unsigned long)__isr##x, 0x08, 0x8E);
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
void IDTX86::initISR()
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

void IDTX86::remapIRQ()
{
	Port::out(0x20, 0x11);
	Port::out(0xA0, 0x11);
	Port::out(0x21, 0x20);
	Port::out(0xA1, 0x28);
	Port::out(0x21, 0x04);
	Port::out(0xA1, 0x02);
	Port::out(0x21, 0x01);
	Port::out(0xA1, 0x01);
	Port::out(0x21, 0x0);
	Port::out(0xA1, 0x0);
}

#define IRQ_GATE_DEF(x) extern "C" void __irq##x();
#define IRQ_GATE(x) setGate(x+32, (unsigned long)__irq##x, 0x08, 0x8E);
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
void IDTX86::initIRQ()
{
	remapIRQ();

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
	if (r==NULL) {
		VideoX86 tmp;
		//tmp.clear();
		tmp.printf("ERROR! ISR, regs. \n");

		Platform p;
		p.state()->seizeInterrupts();
		p.state()->halt();
	}

	void (*routine)(Regs *r);
	routine = NULL;

	routine = IDTX86::get()->routine(r->int_no - 32);
	if (routine!=NULL) {
		routine(r);
	}

	IDTX86::Handler *handler = IDTX86::handler(r->int_no);
	while (handler!=NULL) {
		handler->high_half(r->int_no, handler->data);
		//handler->bottom_half(r->int_no, handler->data); //FIXME
		handler = handler->next;
	}

	if (r->int_no >= 40) {
		Port::out(0xA0, 0x20);
	}

	Port::out(0x20, 0x20);
}

extern uint32_t debug_ptr;
extern uint32_t debug_ptr_cr2;
extern "C" void isr_handler(Regs * r)
{
	if (r==NULL) {
		VideoX86 tmp;
		tmp.clear();
		tmp.printf("ERROR! ISR, regs. \n");

		Platform p;
		p.state()->seizeInterrupts();
		p.state()->halt();
	}
	if (r->int_no == 0xE) {
		VideoX86 tmp;
		tmp.printf("\nERROR! Page fault! %x EIP: %x  \n", debug_ptr_cr2, r->eip);
		Platform p;
		p.state()->seizeInterrupts();
		p.state()->halt();
	}
	if (r->int_no == 6) {
		VideoX86 tmp;
		tmp.printf("\nERROR! Invalid instruction: %x\n", debug_ptr);
		Platform p;
		p.state()->seizeInterrupts();
		p.state()->halt();
	}
	if (r->int_no < 32) {
		// Got it
		VideoX86 tmp;
		//tmp.clear();
		tmp.printf("ERROR! Exception %d\n",r->int_no);

		Platform p;
		p.state()->seizeInterrupts();
		p.state()->halt();
	}

	IDTX86::Handler *handler = IDTX86::handler(r->int_no);
	while (handler!=NULL) {
		handler->high_half(r->int_no, handler->data);
		//handler->bottom_half(r->int_no, handler->data); //FIXME
		handler = handler->next;
	}
}
