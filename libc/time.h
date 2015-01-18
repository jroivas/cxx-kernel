#ifndef __TIME_H
#define __TIME_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t time_t;
time_t time(time_t *t);

#ifdef __cplusplus
}
#endif

#endif
