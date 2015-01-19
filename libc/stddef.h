#ifndef __STDDEF_H
#define __STDDEF_H

#include <stdarg.h>
#include <types.h>
#include <wchar.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#endif
