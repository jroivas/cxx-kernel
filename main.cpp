#include "cxa.h"
#include "paging.h"
#include "gdt.h"
#include "types.h"
#include "kernel.h"

extern "C" void _main(unsigned long multiboot, unsigned long magic)
{
	(void)magic;
	if (magic == 0x1BADB002) {
		return;
	}

#if 0
	unsigned short *tmp = (unsigned short *)(0xB8000);
	*tmp = 0x1744; //D
#endif
	gdt_init();


	extern void (* start_ctors)();
	extern void (* end_ctors)();
	void (**constructor)() = & start_ctors;
	while (constructor<&end_ctors) {
		//*(tmp) = 0x2730+((unsigned int)constructor%10);
		((void (*) (void)) (*constructor)) ();
		constructor++;
	}

	paging_init((MultibootInfo *)multiboot);

	/* Run the kernel */
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
	__cxa_finalize(0);
#endif
}
