#ifndef __STDDEF_H
#define __STDDEF_H

#include <stdarg.h>
#include <types.h>
#include <wchar.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#endif
