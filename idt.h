#ifndef IDT_H
#define IDT_H

#include "regs.h"

#define IRQ_ROUTINES 16

typedef void (*routine_t)(Regs *r);

class IDT
{
public:
	static IDT *getInstance();
	void initISR();
	void initIRQ();
	static routine_t routine(unsigned int i);
	void installRoutine(unsigned int i, void (*handler)(Regs *r));

	struct Ptr {
		unsigned short limit;
		unsigned int base;
	} __attribute__((packed));
	struct Entry {
		unsigned short base_low;
		unsigned short sel;
		unsigned char always0;
		unsigned char flags;
		unsigned short base_high;
	} __attribute__((packed));

private:
	IDT();

//	Entry idt[256];
//	Ptr idtp;

	routine_t routines[IRQ_ROUTINES];

	void load();
	void setGate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
	void remapIRQ();
};

#endif
