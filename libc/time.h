#ifndef __TIME_H
#define __TIME_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timeval {
    time_t tv_sec;
    long tv_usec;
};
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

time_t time(time_t *t);

#ifdef __cplusplus
}
#endif

#endif
