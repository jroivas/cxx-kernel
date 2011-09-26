#include "types.h"
#include "mm.h"

void *operator new(size_t size)
{
	//return kmalloc(size);
	return MM::instance()->alloc(size);
}

void *operator new[](size_t size)
{
	//return kmalloc(size);
	return MM::instance()->alloc(size);
}

void operator delete(void *p)
{
	MM::instance()->free(p);
	//kfree(p);
}
 
void operator delete[](void *p)
{
	//kfree(p);
	MM::instance()->free(p);
}
