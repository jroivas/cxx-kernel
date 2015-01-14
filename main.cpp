#include "cxa.h"
#include "paging.h"
#include "gdt.h"
#include "types.h"
#include "kernel.h"
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
        uint32          volatile tim0;          //0
        uint32          volatile tim1;          //4
        uint32          volatile tim2;          //8
        uint32          volatile d;             //c
        uint32          volatile upbase;        //10
        uint32          volatile f;             //14
        uint32          volatile g;             //18
        uint32          volatile control;       //1c
} PL110MMIO;

extern "C" void _main(unsigned long multiboot, unsigned long magic)
{
/*
#ifndef NATIVE_LINUX
    long base = 0x20200000;
    *((long *)(base+4)) = (1<<18);
#endif
*/
#if 0
    //while (1) {
    for (int aa=1; aa<5; aa++) {
        *((uint32_t *)(base+40)) = (1<<16);
        volatile uint32_t pp=0x3F0000;
        while (pp>10) pp--;
        *((uint32_t *)(base+28)) = (1<<16);
        pp=0x3F0000;
        while (pp>10) pp--;
    }
    {
        volatile uint32_t pp=0x3F0000;
        while (pp>10) pp--;
    }
#endif

    (void)magic;
    if (magic == 0x1BADB002) {
            return;
    }


#if 0
	unsigned short *tmp = (unsigned short *)(0xB8000);
	*tmp = 0x1744; //D
#endif
	//while (1) {
#if 0
	for (int aa=1; aa<5; aa++) {
		*((uint32_t *)(base+40)) = (1<<16);
		volatile uint32_t pp=0xF0000;
		while (pp>10) pp--;
		*((uint32_t *)(base+28)) = (1<<16);
		pp=0xF0000;
		while (pp>10) pp--;
	}
	{
		volatile uint32_t pp=0x3F0000;
		while (pp>10) pp--;
	}
#endif
    gdt_init();

    extern void (* start_ctors)();
    extern void (* end_ctors)();

    extern void (* init_array_start)();
    extern void (* init_array_end)();

    if (start_ctors!=NULL) {
        void (**constructor)() = & start_ctors;
        while (constructor<&end_ctors) {
            ((void (*) (void)) (*constructor)) ();
            ++constructor;
        }
    }
    if (init_array_start != NULL) {
        void (**constructor)() =  &init_array_start;
        while (constructor < &init_array_end) {
            ((void (*) (void)) (*constructor)) ();
            ++constructor;
        }
    }

#if 0
	//while (1) {
	for (int aa=1; aa<5; aa++) {
		*((uint32_t *)(base+40)) = (1<<16);
		volatile uint32_t pp=0x2F000;
		while (pp>10) pp--;
		*((uint32_t *)(base+28)) = (1<<16);
		pp=0x2F000;
		while (pp>10) pp--;
	}
	{
		volatile uint32_t pp=0x2F000;
		while (pp>10) pp--;
	}
#endif

    //unsigned short *tmp = (unsigned short *)(0xB8000);
    //*tmp = 0x1745; //E
    paging_init((MultibootInfo *)multiboot);

    //*tmp = 0x1746; //F
#if 0
    for (int aa=1; aa<5; aa++) {
            *((uint32_t *)(base+40)) = (1<<16);
            volatile uint32_t pp=0x3F0000;
            while (pp>10) pp--;
            *((uint32_t *)(base+28)) = (1<<16);
            pp=0x3F0000;
            while (pp>10) pp--;
    }
#endif

	/* Run the kernel */

#if 0
	for (int aa=1; aa<5; aa++) {
		*((uint32_t *)(base+40)) = (1<<16);
		volatile uint32_t pp=0x2F000;
		while (pp>10) pp--;
		*((uint32_t *)(base+28)) = (1<<16);
		pp=0x2F000;
		while (pp>10) pp--;
	}
	*((uint32_t *)(base+40)) = (1<<16);
	while(1) ;
#endif
    Kernel *k = new Kernel();
    k->run();

    delete k;
}

extern "C" void _atexit()
{
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
	__cxa_finalize(0);
#endif
#endif
}
