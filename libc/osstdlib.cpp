#include "osstdlib.h"
#include "stdio.h"

void exit(int status)
{
    //FIXME
    printf("Error: %d halting\n", status);
    while (1) ;
}
