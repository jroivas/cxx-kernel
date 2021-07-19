#ifndef __SYSCALL_TIME_H
#define  __SYSCALL_TIME_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

int syscall_clock_gettime(clockid_t clockid, struct timespec *tp);

#ifdef __cplusplus
}
#endif

#endif
