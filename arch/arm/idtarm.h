#ifndef IDT_ARM_H
#define IDT_ARM_H

#include "regs.h"
#include "types.h"
#include "idt.h"

#define IRQ_ROUTINES 1
#define ISR_HANDLERS 6

class IDTARM : public IDT
{
public:
    IDTARM();
    void initISR();
    void initIRQ();
    void installRoutine(unsigned int i, idt_routine_t);
    void uninstallRoutine(unsigned int i);
    void installHandler(unsigned int i, idt_handler_t high, idt_handler_t bottom, void *data);

protected:
    idt_routine_t getRoutine(unsigned int i);

private:
};

#endif
