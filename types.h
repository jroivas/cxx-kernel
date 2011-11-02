#ifndef TYPES_H
#define TYPES_H

#define KERNEL_VIRTUAL 0xC0000000

#ifndef NULL
#define NULL 0
#endif
typedef unsigned int size_t;
typedef unsigned char* ptr8_t;
typedef unsigned char  ptr8_val_t;
typedef unsigned long* ptr32_t;
typedef unsigned long  ptr32_val_t;
typedef unsigned long* ptr_t;
typedef unsigned long phys_ptr_t;
typedef unsigned long ptr_val_t;
typedef int off_t;
typedef char  int8_t;
typedef unsigned char  uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int  int32_t;
typedef unsigned int  uint32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;

#define PAGE_CNT 1024
#define PAGING_SIZE (sizeof(ptr32_t)*PAGE_CNT) // A little bit more portable
#define PAGE_SIZE PAGING_SIZE

#include "operators.h"


#endif
