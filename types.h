#ifndef TYPES_H
#define TYPES_H

#define KERNEL_VIRTUAL 0xC0000000

#define NULL 0
typedef unsigned int size_t;
typedef unsigned char* ptr8_t;
typedef unsigned char  ptr8_val_t;
typedef unsigned long* ptr32_t;
typedef unsigned long  ptr32_val_t;

#define PAGE_CNT 1024
#define PAGING_SIZE (sizeof(ptr32_t)*PAGE_CNT) // A little bit more portable
#define PAGE_SIZE PAGING_SIZE

#include "operators.h"


#endif
