#include "assert.h"

void __assert(const char *reason)
{
    printf("ASSERT FAILURE: %s\nHalting...\n", reason);
    while(1) ;
}
