#include "x86.h"

void cli()
{
    asm volatile("cli");
}

void sti()
{
    asm volatile("sti");
}

void hlt()
{
    asm volatile("hlt");
}
