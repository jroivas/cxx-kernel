#ifndef MEMCOPY_H
#define MEMCOPY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "types.h"

void *memcpy(void *dest, const void *src, unsigned int n);
void *memmove(void *dest, const void *src, unsigned int n);
void *memset(void *s, int c, size_t n);

#ifdef __cplusplus
}
#endif

#endif
