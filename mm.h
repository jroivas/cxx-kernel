#ifndef MM_H
#define MM_H

#include "types.h"

void *kmalloc(size_t size);
void *kcalloc(unsigned int cnt, size_t size);
void kfree(void *p);


#endif
