#ifndef GDT_H
#define GDT_H

void gdt_init();
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void *getTss0();

#endif
