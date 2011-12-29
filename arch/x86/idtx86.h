#ifndef IDT_X86_H
#define IDT_X86_H

#include "regs.h"
#include "types.h"
#include "idt.h"

#define IRQ_ROUTINES 16
#define ISR_HANDLERS 256

class IDTX86 : public IDT
{
public:
	class Handler
	{
	public:
		Handler() { num=0; high_half=NULL; bottom_half=NULL; data=NULL; next=NULL; }
		Handler(unsigned int i) { num = i; high_half=NULL; bottom_half=NULL; data=NULL; next=NULL; }

		unsigned int num;
		idt_handler_t high_half;
		idt_handler_t bottom_half;
		void *data;
		Handler *next;
	};
	IDTX86();
	void initISR();
	void initIRQ();
	//static idt_routine_t routine(unsigned int i);
	static Handler *handler(unsigned int i);
	void installRoutine(unsigned int i, idt_routine_t);
	void uninstallRoutine(unsigned int i);
	void installHandler(unsigned int i, idt_handler_t high, idt_handler_t bottom, void *data);

	struct Ptr {
		unsigned short limit;
		unsigned int base;
	} __attribute__((packed));

protected:
	idt_routine_t getRoutine(unsigned int i);
	Handler *getHandler(unsigned int i);

private:
	struct Entry {
		unsigned short base_low;
		unsigned short sel;
		unsigned char always0;
		unsigned char flags;
		unsigned short base_high;
	} __attribute__((packed));

	Entry idt[256];

	idt_routine_t routines[IRQ_ROUTINES];
	Handler *handlers[ISR_HANDLERS];

	void load();
	void setGate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
	void remapIRQ();
};

#endif
