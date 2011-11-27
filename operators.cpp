#include "types.h"
#include "mm.h"
#include "paging.h"

static Paging __operator_paging;
//Paging __operator_paging;

void *operator new(size_t size)
{
	if (__operator_paging.isOk()) {
		return MM::instance()->alloc(size);
	} else {
		//return __operator_paging.allocStatic(size, NULL);
		__operator_paging.lock();
		void *res = __operator_paging.allocStatic(size, NULL);
		__operator_paging.unlock();
		return res;
	}
}

void *operator new[](size_t size)
{
	if (__operator_paging.isOk()) {
		return MM::instance()->alloc(size);
	} else {
		//return __operator_paging.allocStatic(size, NULL);
		__operator_paging.lock();
		void *res = __operator_paging.allocStatic(size, NULL);
		__operator_paging.unlock();
		return res;
	}
}

void operator delete(void *ptr)
{
	if (__operator_paging.isOk()) {
		MM::instance()->free(ptr);
	}
}
 
void operator delete[](void *ptr)
{
	if (__operator_paging.isOk()) {
		MM::instance()->free(ptr);
	}
}

void *operator new(size_t size, ptr_t addr)
{
	__operator_paging.lock();
#if 0
	if (__operator_paging.isOk()) {
		void *tmp = MM::instance()->alloc(size);
		*addr = (ptr_val_t)tmp;
		return tmp;
	} else {
		return __operator_paging.allocStatic(size, addr);
	}
#else
	void *res = __operator_paging.allocStatic(size, addr);
	__operator_paging.unlock();
	return res;
#endif
}
