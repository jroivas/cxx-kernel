#ifndef IDT_X86_H
#define IDT_X86_H

#include "regs.h"
#include "types.h"
#include "idt.h"

#define INTERRUPTS 256
#define IRQ_ROUTINES 256
#define ISR_HANDLERS 256

class IDTX86 : public IDT
{
public:
    class Handler
    {
    public:
        Handler()
            : num(0),
            high_half(nullptr),
            bottom_half(nullptr),
            data(nullptr),
            next(nullptr)
        {
        }
        Handler(unsigned int i)
            : num(i),
            high_half(nullptr),
            bottom_half(nullptr),
            data(nullptr),
            next(nullptr)
        {
        }

        unsigned int num;
        idt_handler_t high_half;
        idt_handler_t bottom_half;
        void *data;
        Handler *next;
    };
    IDTX86();
    void initISR();
    void initIRQ();

    static Handler *handler(unsigned int i);
    void installRoutine(unsigned int i, idt_routine_t);
    void uninstallRoutine(unsigned int i);
    void installHandler(unsigned int i, idt_handler_t high, idt_handler_t bottom, void *data);

    struct Ptr {
        unsigned short limit;
        unsigned int base;
    } __attribute__((packed));

protected:
    inline idt_routine_t getRoutine(unsigned int i) const
    {
        return routines[i];
    }
    inline Handler *getHandler(unsigned int i) const
    {
        return handlers[i];
    }

private:
    struct Entry {
        unsigned short base_low;
        unsigned short sel;
        unsigned char always0;
        unsigned char flags;
        unsigned short base_high;
    } __attribute__((packed));

    Entry idt[INTERRUPTS];

    idt_routine_t routines[IRQ_ROUTINES];
    Handler *handlers[ISR_HANDLERS];

    void load();
    void setGate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
    void remapIRQ();
};

#endif
