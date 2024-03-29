#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>

#ifndef NULL
#define NULL 0
#endif

#ifdef ARCH_LINUX
#include <stdint.h>
#endif

#if defined(ARCH_ARM)
typedef unsigned int operator_size_t;
#elif defined(__clang__)
typedef unsigned int operator_size_t;
#else
typedef size_t operator_size_t;
#endif

#if 0
#if __i386__
typedef unsigned int size_t;
typedef int ssize_t;
typedef unsigned int uintptr_t;
typedef int intptr_t;
#elif __x86_64__
typedef unsigned long size_t;
typedef long ssize_t;
typedef unsigned long uintptr_t;
typedef long intptr_t;
#endif
#endif

typedef unsigned char* ptr8_t;
typedef unsigned char  ptr8_val_t;
typedef unsigned long* ptr32_t;
typedef unsigned long  ptr32_val_t;
typedef unsigned long* ptr_t;
typedef unsigned long phys_ptr_t;
typedef unsigned long ptr_val_t;
#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
#endif

#if 0
typedef signed char  int8_t;
typedef unsigned char  uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int  int32_t;
typedef unsigned int  uint32_t;

#if __x86_64__
typedef long int int64_t;
typedef unsigned long int uint64_t;
#else
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#endif

#if 0
typedef uint64_t time_t;

typedef uint32_t dev_t;
typedef uint32_t ino_t;
typedef uint16_t mode_t;
typedef uint16_t nlink_t;
typedef uint16_t uid_t;
typedef uint16_t gid_t;
#ifndef ARCH_LINUX
typedef uint32_t off_t;
#endif
typedef uint32_t blksize_t;
typedef uint64_t blkcnt_t;
#endif
#endif

#if defined(ARCH_LINUX) || defined(ARCH_ARM) || defined(ARCH_x86)
#include "operators.h"
#endif

#endif
