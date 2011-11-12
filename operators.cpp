#include "types.h"
#include "mm.h"
#include "paging.h"

void *operator new(size_t size)
{
	Paging p;
	if (p.isOk()) {
		return MM::instance()->alloc(size);
	} else {
		return p.allocStatic(size, NULL);
	}
}

void *operator new[](size_t size)
{
	Paging p;
	if (p.isOk()) {
		return MM::instance()->alloc(size);
	} else {
		return p.allocStatic(size, NULL);
	}
}

void operator delete(void *p)
{
	MM::instance()->free(p);
}
 
void operator delete[](void *p)
{
	MM::instance()->free(p);
}

void *operator new(size_t size, ptr_t addr)
{
	Paging p;
	return p.allocStatic(size, addr);
}
