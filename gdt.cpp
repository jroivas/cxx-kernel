#include "gdt.h"

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

struct gdt_entry_t gdt[6];
struct gdt_ptr_t gdt_ptr;

extern "C" void gdt_flush();

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

tss_t tss0;

extern "C" unsigned long get_esp();

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;
 
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);
 
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

void gdt_init()
{
	gdt_ptr.limit = (sizeof(struct gdt_entry_t) * 6) - 1;
	gdt_ptr.base = (unsigned int)&gdt;

        tss0.ss0 = (unsigned short)0x10;
        tss0.esp0 = get_esp();
        tss0.iopb = (unsigned short)sizeof(tss_t) - 1;
 
	gdt_set_gate(0, 0, 0, 0, 0);
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
        gdt_set_gate(5, (unsigned long) &tss0, sizeof(tss_t) - 1, 0x89, 0x40);
 
	gdt_flush();

        asm volatile("ltr %%ax": : "a" (0x28));
}
