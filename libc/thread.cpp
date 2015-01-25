//#include <thread.hh>
#include <types.h>

extern "C"
int __clone(int (*)(void *), void *, int, void *, ...)
{
    //TODO
    return -1;
}

extern "C"
void __unmapself(void *, size_t)
{
    //TODO
}
