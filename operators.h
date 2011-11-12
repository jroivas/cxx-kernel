#ifndef OPERATORS_H
#define OPERATORS_H

#include "types.h"

#ifdef __cplusplus
//void *operator new(size_t size);
void *operator new(size_t size, ptr_t addr);

#if 0
inline void *operator new(size_t, void *p) throw() { return p; }
inline void *operator new[](size_t, void *p) throw() { return p; }
inline void operator delete  (void *, void *) throw() { };
inline void operator delete[](void *, void *) throw() { };
#endif
#endif

#endif
