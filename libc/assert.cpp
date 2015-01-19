#include "assert.h"

void __assert(char *reason)
{
    printf("ASSERT FAILURE: %s\nHalting...\n", reason);
    while(1) ;
}
