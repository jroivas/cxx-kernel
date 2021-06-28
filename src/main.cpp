#include "cxa.h"
#include "paging.h"
#include "gdt.h"
#include "types.h"
#include "kernel.h"
#include "platform.h"
#include "acpi.h"

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

extern uint32_t localId();

#define STACK_SIZE 0x4000 * 64
uint8_t __stack_bottom[STACK_SIZE] = {};
uint32_t __stack_top = (uint32_t)__stack_bottom + STACK_SIZE;

extern "C" void _smp_main()
{
    gdt_load();
    pagingEnableSmp();

    //Platform *platform = new Platform();
    uint32_t id = acpiCpuLocalId();
    Platform::video()->printf("CPU initted: %u\n", id);

#if 0
    uint32_t esp;
    asm volatile("mov %%esp, %[Var]" : [Var] "=r" (esp));
    Platform::video()->printf("ESP: 0x%8x\n", esp);
#endif

    // TODO Allow scheduling on sMP CPUs
    while(true) ;
}

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

    Platform *platform = new Platform();

    // Initialize paging
    //paging_init((MultibootInfo *)multiboot);

    /* Run the kernel */

    Kernel *k = new Kernel(platform, (MultibootInfo *)multiboot);
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
