#include "string.h"

typedef void (*ctor_type_t)();

ctor_type_t *start_ctors = NULL;
ctor_type_t *end_ctors = NULL;

extern "C" void _main(unsigned long multiboot, unsigned long magic);
extern "C" void loaderstart() {
	_main(0,0);
}

extern "C" unsigned int get_esp() {
	return 0;
}

extern "C" void gdt_flush() {
}

extern "C" void *memcpy_opt(void *dest, void *src, size_t n) {
	return Mem::copy(dest, src,n);
}

extern "C" int main(int argc, char**argv)
{
	(void)argc;
	(void)argv;
	loaderstart();
	return 0;
}
