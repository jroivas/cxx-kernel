#ifndef OPERATORS_H
#define OPERATORS_H

#include "types.h"
#include <sys/types.h>

#ifdef __cplusplus
void *operator new(operator_size_t size, ptr_t addr);
#endif

#endif
