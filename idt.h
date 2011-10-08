#ifndef IDT_H
#define IDT_H

#include "regs.h"

#define IRQ_ROUTINES 16

typedef void (*routine_t)(Regs *r);

class IDT
{
public:
	static IDT *get();
	void initISR();
	void initIRQ();
	static routine_t routine(unsigned int i);
	void installRoutine(unsigned int i, void (*handler)(Regs *r));
	void uninstallRoutine(unsigned int i);

	struct Ptr {
		unsigned short limit;
		unsigned int base;
	} __attribute__((packed));

private:
	IDT();
	struct Entry {
		unsigned short base_low;
		unsigned short sel;
		unsigned char always0;
		unsigned char flags;
		unsigned short base_high;
	} __attribute__((packed));

	Entry idt[256];

	routine_t routines[IRQ_ROUTINES];

	void load();
	void setGate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
	void remapIRQ();
};

#endif
