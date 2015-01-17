#include "types.h"
#include "mm.h"

void *operator new(size_t size)
{
	return kmalloc(size);
}

void *operator new[](size_t size)
{
	return kmalloc(size);
}

void operator delete(void *p)
{
	kfree(p);
}
 
void operator delete[](void *p)
{
	kfree(p);
}
