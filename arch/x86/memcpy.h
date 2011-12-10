#ifndef MEMCPY_X86_H
#define MEMCPY_X86_H

#include "string.h"

extern "C" bool mmx_has();
extern "C" bool sse_has();
extern "C" bool sse2_has();

extern "C" void mmx_memcpy(unsigned char *d, unsigned char *s, unsigned int cnt);
extern "C" void *memcpy_opt(void *dest, const void *src, unsigned int n);

#endif
