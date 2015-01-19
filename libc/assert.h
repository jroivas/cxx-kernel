#ifndef __ASSERT_H
#define __ASSERT_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void __assert(const char *reason);

#define assert(x) ((x) ? 0 : (__assert(#x)))

#ifdef __cplusplus
}
#endif

#endif
