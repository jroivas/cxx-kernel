#include "cxa.h"
#include "types.h"

#ifndef ARCH_LINUX

void *__dso_handle = 0;

void __cxa_pure_virtual()
{
}


#ifdef ARCH_ARM
extern "C" int __aeabi_atexit(void* object, void (*destroyer)(void*), void* dso_handle)
{
    return __cxa_atexit(destroyer, object, dso_handle);
}
#endif
#endif
