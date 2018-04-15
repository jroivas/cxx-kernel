#include "cxa.h"
#include "paging.h"
#include "gdt.h"
#include "types.h"
#include "kernel.h"

#if 0
#include "arch/arm/gpio.h"

#define PL110_CR_EN             0x001
#define PL110_CR_PWR            0x800
#define PL110_IOBASE            0xc0000000
#define PL110_PALBASE           (PL110_IOBASE + 0x200)

typedef unsigned int            uint32;
typedef unsigned char           uint8;
typedef unsigned short          uint16;

typedef struct _PL110MMIO
{
    uint32  volatile tim0;          //0
    uint32  volatile tim1;          //4
    uint32  volatile tim2;          //8
    uint32  volatile d;             //c
    uint32  volatile upbase;        //10
    uint32  volatile f;             //14
    uint32  volatile g;             //18
    uint32  volatile control;       //1c
} PL110MMIO;
#endif

extern "C" void _main(unsigned long multiboot, unsigned long magic)
{
    if (magic == 0x1BADB002) {
        return;
    }

    gdt_init();

    // Constructors and init array
    extern void (* start_ctors)();
    extern void (* end_ctors)();

    extern void (* init_array_start)();
    extern void (* init_array_end)();

    if (start_ctors!=nullptr) {
        void (**constructor)() = & start_ctors;
        while (constructor<&end_ctors) {
            ((void (*) (void)) (*constructor)) ();
            ++constructor;
        }
    }
    if (init_array_start != nullptr) {
        void (**constructor)() =  &init_array_start;
        while (constructor < &init_array_end) {
            ((void (*) (void)) (*constructor)) ();
            ++constructor;
        }
    }

    // Initialize paging
    paging_init((MultibootInfo *)multiboot);

    /* Run the kernel */

    Kernel *k = new Kernel();
    k->run();

    delete k;
}

extern "C" void _atexit()
{
    // We probably do not want to run destructors
    // or finit list
#if 0
    extern void (* start_dtors)();
    extern void (* end_dtors)();
    void (**destructor)() = & start_dtors;
    while (destructor<&end_dtors) {
        ((void (*) (void)) (*destructor)) ();
        destructor++;
    }
#else
#ifndef ARCH_LINUX
    //__cxa_finalize(0);
#endif
#endif
}
