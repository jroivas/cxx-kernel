#ifndef IDT_H
#define IDT_H

#include "regs.h"

typedef void (*routine_t)(Regs *r);

class IDT
{
public:
	static IDT *get();
	virtual void initISR() = 0;
	virtual void initIRQ() = 0;
	virtual void installRoutine(unsigned int i, void (*handler)(Regs *r)) = 0;
	routine_t routine(unsigned int i);

protected:
	IDT() { }
	virtual routine_t getRoutine(unsigned int i) = 0;
};
#endif
