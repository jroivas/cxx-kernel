#ifndef X86_H
#define X86_H

static inline void cli()
{
	asm volatile("cli");
}

static inline void sti()
{
	asm volatile("sti");
}

static inline void hlt()
{
	asm volatile("hlt");
}

#endif
