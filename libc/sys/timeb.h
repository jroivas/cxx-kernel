#ifndef __TIMEB_H
#define __TIMEB_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timeb
{
    time_t time;
    unsigned short int millitm;
    short int timezone;
    short int dstflags;
};

#ifdef __cplusplus
}
#endif

#endif
