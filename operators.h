#ifndef OPERATORS_H
#define OPERATORS_H

#include "types.h"

#if 0
inline void *operator new(size_t, void *p) throw() { return p; }
inline void *operator new[](size_t, void *p) throw() { return p; }
inline void operator delete  (void *, void *) throw() { };
inline void operator delete[](void *, void *) throw() { };
#endif

#endif
