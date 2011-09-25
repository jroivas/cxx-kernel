#include "cxa.h"
#include "paging.h"
#include "gdt.h"
#include "types.h"
#include "kernel.h"

//extern main(unsigned long multiboot, unsigned long magic);
extern int main();

extern "C" void _main(unsigned long multiboot, unsigned long magic)
{
	(void)magic;
	if (magic == 0x1BADB002) {
		return;
	}

#if 0
	unsigned short *tmp = (unsigned short *)(KERNEL_VIRTUAL+0xB8000);
	*tmp = 0x1744; //D
#endif

	gdt_init();
	paging_init((MultibootInfo *)multiboot);

	extern void (* start_ctors)();
	extern void (* end_ctors)();
	void (**constructor)() = & start_ctors;
	while (constructor<&end_ctors) {
		((void (*) (void)) (*constructor)) ();
		constructor++;
	}

	Kernel k;
	k.run();
	//main();
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
	__cxa_finalize(0);
#endif
}
