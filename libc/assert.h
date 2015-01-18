#ifndef __ASSERT_H
#define __ASSERT_H

#include <stdio.h>

#define assert(x)\
    if (!(x)) {\
        printf("ASSERT FAILURE: %s\nHalting...\n", #x);\
        while(1) ;\
    }

#endif
