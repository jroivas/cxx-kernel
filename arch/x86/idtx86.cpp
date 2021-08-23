#include "idtx86.h"
#include "port.h"
#include "types.h"
#include "string.hh"
#include "videox86.h"
#include "platform.h"
#include "uart.hh"

extern "C" void idt_load();

IDTX86::Ptr idt_idtp;

IDTX86::IDTX86() : IDT()
{
    idt_idtp.limit = (sizeof(Entry) * INTERRUPTS) - 1;
    idt_idtp.base = (unsigned int)&idt;

    Mem::set(&idt, 0, sizeof(Entry) * INTERRUPTS);

    for (int i = 0; i < IRQ_ROUTINES; i++) {
        routines[i] = nullptr;
    }
    for (int i = 0; i < ISR_HANDLERS; i++) {
        handlers[i] = nullptr;
    }

    load();
}

void IDTX86::load()
{
    idt_load();
}

void IDTX86::installRoutine(unsigned int i, idt_routine_t handler)
{
    routines[i] = handler;
}

void IDTX86::uninstallRoutine(unsigned int i)
{
    routines[i] = nullptr;
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
    if (handlers[i]==nullptr) {
        handlers[i] = h;
    } else {
        Handler *p = handlers[i];
        while (p->next != nullptr) {
            p = p->next;
        }
        p->next = h;
    }
}

IDTX86::Handler *IDTX86::handler(unsigned int i)
{
    if (get() == nullptr) return nullptr;
    return ((IDTX86*)get())->getHandler(i);
}

void IDTX86::setGate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = ((base >> 16) & 0xFFFF);

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
IRQ_GATE_DEF(121);
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
    IRQ_GATE(121);
}

extern "C" int irq_handler(Regs &regs)
{
    if (regs.int_no < 32) {
        Port::out(0x20, 0x20);
        return -1;
    }

#if 1
    int (*routine)(Regs *r);
    routine = nullptr;

    routine = IDTX86::get()->routine(regs.int_no);
    int res = 0;
    if (routine != nullptr) {
        res = routine(&regs);
    }

#if 1
    IDTX86::Handler *handler = IDTX86::handler(regs.int_no);
    while (handler != nullptr) {
        if (handler->high_half != nullptr) {
            handler->high_half(regs.int_no, handler->data);
        }
        //handler->bottom_half(r->int_no, handler->data); //FIXME
        handler = handler->next;
    }
#endif

    if (regs.int_no >= 40) {
        Port::out(0xA0, 0x20);
    }

    Port::out(0x20, 0x20);
    return res;
#endif
}

#ifdef ARCH_x86
typedef struct StackFrame {
  ptr_t ebp;
  ptr_t eip;
} StackFrame;
#endif

static void stacktrace(uint32_t frames)
{
#ifdef ARCH_x86
    VideoX86 tmp;
    StackFrame *frame = nullptr;
    Paging p;

#ifdef __clang__
    asm("movl %%ebp,%0" : "=r"(frame));
#else
    register ptr_t *ebp asm("ebp");
    frame = (StackFrame*)ebp;
#endif

    // It's possible pages are not mapped, map them now
    p.identityMap((ptr_val_t)frame);

    //tmp.printf("Stacktrace:\n");
    uart_print("Stacktrace\n");
    for (uint32_t i = 0; frame && i < frames; ++i) {
        uart_print_uint64((ptr_val_t)frame);
        uart_print_uint64((uint64_t)frame->eip);
        uart_print_uint64_hex((uint64_t)frame->eip);
#if 0
        tmp.printf("  0x%8x ", (ptr_val_t)frame);
        tmp.printf("  0x%x\n", frame->eip);
#endif
        if (!frame->ebp)
            break;
        p.identityMap((ptr_val_t)frame->ebp);
        frame = (StackFrame*)frame->ebp;
    }
#else
    (void)frames;
#endif
}

extern ptr_val_t debug_ptr;
extern ptr_val_t debug_ptr_cr2;
extern ptr_val_t debug_ptr_eip;
extern "C" int isr_handler(Regs &regs)
{
    (void)stacktrace;
    if (regs.int_no == 0xE) {
#if 0
        uart_print("0xE:\n");
        uart_print_uint64(regs.eip);
        uart_print_uint64(regs.eax);
        uart_print_uint64_hex(debug_ptr_eip);
#endif
        regs.dump();
        Platform p;
        p.state()->seizeInterrupts();
        while (1);
        p.state()->halt();
        return -1;
    }
    if (regs.int_no == 6) {
        VideoX86 tmp;
        tmp.printf("\nERROR! Invalid instruction: %x: EIP: %x\n", debug_ptr, regs.eip);
        regs.dump();
        //stacktrace(10);
        Platform p;
        p.state()->seizeInterrupts();
        p.state()->halt();
        return -1;
    }
    if (regs.int_no < 32) {
        // Got it
        VideoX86 tmp;
        tmp.printf("ERROR! Exception %d EIP: %x\n",regs.int_no, regs.eip);

        Platform p;
        p.state()->seizeInterrupts();
        p.state()->halt();
        return -1;
    }

    IDTX86::Handler *handler = IDTX86::handler(regs.int_no);
    while (handler != nullptr) {
        handler->high_half(regs.int_no, handler->data);
        //handler->bottom_half(r->int_no, handler->data); //FIXME
        handler = handler->next;
    }
    return 0;
}

extern ptr_val_t irq_tmp_validate;
extern ptr_val_t irq_esp_validate;
extern "C" void irq_handler_error()
{
    uart_print("IRQ STACK mistmatch:");
    uart_print_uint64_hex(irq_tmp_validate);
    uart_print_uint64_hex(irq_esp_validate);
    while (1);
}
