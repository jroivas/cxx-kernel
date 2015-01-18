#ifndef IDT_H
#define IDT_H

#include "regs.h"

typedef int (*idt_routine_t)(Regs *r);
typedef void (*idt_handler_t)(unsigned int num, void *data);

class IDT
{
public:
    static IDT *get();
    virtual void initISR() = 0;
    virtual void initIRQ() = 0;
    virtual void installRoutine(unsigned int i, idt_routine_t) = 0;
    virtual void uninstallRoutine(unsigned int i) = 0;
    virtual void installHandler(unsigned int i, idt_handler_t high, idt_handler_t bottom, void *data) = 0;
    idt_routine_t routine(unsigned int i);

protected:
    IDT() { }
    virtual idt_routine_t getRoutine(unsigned int i) = 0;
};
#endif
