#ifndef GDT_H
#define GDT_H

#include "types.h"

void gdt_init();
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void *getTss0();

ptr_val_t get_gs_base();
void set_gs_base(ptr_val_t base);
ptr_val_t get_fs_base();
void set_fs_base(ptr_val_t base);

#endif
