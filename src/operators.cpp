#include "types.h"
#include "mm.h"
#include "paging.h"

static Paging __operator_paging;

void *operator new(size_t size)
{
    if (__operator_paging.isOk()) {
        return MM::instance()->alloc(size);
    } else {
        return __operator_paging.allocStatic(size, NULL);
    }
}

void *operator new[](size_t size)
{
    if (__operator_paging.isOk()) {
        return MM::instance()->alloc(size);
    } else {
        return __operator_paging.allocStatic(size, NULL);
    }
}

void operator delete(void *ptr)
{
    if (__operator_paging.isOk()) {
        MM::instance()->free(ptr);
    }
}

void operator delete(void *ptr, long unsigned int)
{
    // FIXME
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

void operator delete[](void *ptr, long unsigned int)
{
    // FIXME
    if (__operator_paging.isOk()) {
        MM::instance()->free(ptr);
    }
}

void *operator new(size_t size, ptr_t addr)
{
    return __operator_paging.allocStatic(size, addr);
}
