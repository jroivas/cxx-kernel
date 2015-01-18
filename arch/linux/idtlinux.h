#ifndef IDT_LINUX_H
#define IDT_LINUX_H

#include "regs.h"
#include "types.h"
#include "idt.h"

#define IRQ_ROUTINES 16
#define ISR_HANDLERS 256

class IDTLinux : public IDT
{
public:
    IDTLinux();
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
