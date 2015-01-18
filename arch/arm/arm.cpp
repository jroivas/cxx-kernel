#include "arm.h"
#include "string.hh"

extern "C" unsigned int get_esp()
{
    return 0;
}

extern "C" void gdt_flush()
{
}

extern "C" void tss_flush()
{
}

extern "C" void *memcpy_opt(void *dest, void *src, size_t n) {
    return Mem::copy(dest, src,n);
}
