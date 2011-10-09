#ifndef IDT_X86_H
#define IDT_X86_H

#include "regs.h"
#include "idt.h"

#define IRQ_ROUTINES 16

class IDTX86 : public IDT
{
public:
	IDTX86();
	void initISR();
	void initIRQ();
	static routine_t routine(unsigned int i);
	void installRoutine(unsigned int i, void (*handler)(Regs *r));
	void uninstallRoutine(unsigned int i);

	struct Ptr {
		unsigned short limit;
		unsigned int base;
	} __attribute__((packed));

protected:
	routine_t getRoutine(unsigned int i);
private:
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
