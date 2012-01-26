#include "gdt.h"
#include "types.h"
#include "string.h"

struct gdt_entry_t
{
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr_t
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct gdt_entry_t __gdt[8];
struct gdt_ptr_t __gdt_ptr;

extern "C" void gdt_flush();
extern "C" void tss_flush();

typedef struct tss
{
        unsigned short link;
        unsigned short r_link;
        unsigned long esp0;
        unsigned short ss0;
        unsigned short r_ss0;
        unsigned long esp1;
        unsigned short ss1;
        unsigned short r_ss1;
        unsigned long esp2;
        unsigned short ss2;
        unsigned short r_ss2;
        unsigned long cr3;
        unsigned long eip;
        unsigned long eflags;
        unsigned long eax;
        unsigned long ecx;
        unsigned long edx;
        unsigned long ebx;
        unsigned long esp;
        unsigned long ebp;
        unsigned long esi;
        unsigned long edi;
        unsigned short es;
        unsigned short r_es;
        unsigned short cs;
        unsigned short r_cs;
        unsigned short ss;
        unsigned short r_ss;
        unsigned short ds;
        unsigned short r_ds;
        unsigned short fs;
        unsigned short r_fs;
        unsigned short gs;
        unsigned short r_gs;
        unsigned short ldtr;
        unsigned short r_ldtr;
        unsigned short r_iopb;
        unsigned short iopb;
} tss_t;

static tss_t __tss0;
static tss_t __tss1;

void *getTss0()
{
	return &__tss0;
}

extern "C" unsigned long get_esp();

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	__gdt[num].base_low = (base & 0xFFFF);
	__gdt[num].base_middle = (base >> 16) & 0xFF;
	__gdt[num].base_high = (base >> 24) & 0xFF;
 
	__gdt[num].limit_low = (limit & 0xFFFF);
	__gdt[num].granularity = ((limit >> 16) & 0x0F);
 
	__gdt[num].granularity |= (gran & 0xF0);
	__gdt[num].access = access;
}

void gdt_init()
{
	__gdt_ptr.limit = (sizeof(struct gdt_entry_t) * 7) - 1;
	__gdt_ptr.base = (ptr_val_t)&__gdt;

	Mem::set(&__tss0, 0, sizeof(tss_t));
        __tss0.cs = (unsigned short)0x08;
        __tss0.ds = (unsigned short)0x10;
        __tss0.cs = (unsigned short)0x10;
        __tss0.es = (unsigned short)0x10;
        __tss0.gs = (unsigned short)0x10;
        __tss0.ss0 = (unsigned short)0x10;
        __tss0.esp0 = get_esp();
        __tss0.iopb = (unsigned short)sizeof(tss_t) - 1;

	Mem::set(&__tss1, 0, sizeof(tss_t));
        __tss1.cs = (unsigned short)0x18;
        __tss1.ds = (unsigned short)0x20;
        __tss1.cs = (unsigned short)0x20;
        __tss1.es = (unsigned short)0x20;
        __tss1.gs = (unsigned short)0x20;
        __tss1.ss1 = (unsigned short)0x20;
        __tss1.esp0 = get_esp();
        __tss1.iopb = (unsigned short)sizeof(tss_t) - 1;
 
	gdt_set_gate(0, 0, 0, 0, 0);
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
        gdt_set_gate(5, (unsigned long) &__tss0, sizeof(tss_t) - 1, 0x89, 0x40);
        gdt_set_gate(6, (unsigned long) &__tss1, sizeof(tss_t) - 1, 0xE9, 0x40);
 
	gdt_flush();
	tss_flush();

#ifdef ARCH_X86
        asm volatile("ltr %%ax": : "a" (0x28));
#endif
}
