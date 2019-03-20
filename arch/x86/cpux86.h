#ifndef CPUx86_H
#define CPUx86_H

#include "cpu.h"

class CPUX86 : public CPU
{
public:
    CPUX86() : CPU() { }
    ~CPUX86(){ }

    void initSMP(Platform *p);
};
#endif

