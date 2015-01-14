#ifndef MATH_H
#define MATH_H

#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif
int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);

uint64_t my__udivdi3(uint64_t num, uint64_t den);
uint64_t my__umoddi3(uint64_t num, uint64_t den);
int64_t my__divdi3(int64_t num, int64_t den);
int64_t my__moddi3(int64_t num, int64_t den);

unsigned int __aeabi_uidiv(unsigned int num, unsigned int den);
int __aeabi_idiv(int num, int den);

#ifdef __cplusplus
}
#endif

#endif
