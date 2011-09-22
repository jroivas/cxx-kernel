#include "cxa.h"

extern "C" void _main()
{
	extern void (* start_ctors)();
	extern void (* end_ctors)();
	void (**constructor)() = & start_ctors;
	while (constructor<&end_ctors) {
		((void (*) (void)) (*constructor)) ();
		constructor++;
	}
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
