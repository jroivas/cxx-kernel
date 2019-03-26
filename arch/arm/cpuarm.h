#ifndef CPUARM_H
#define CPUARM_H

#include "cpu.h"
#include "platform.h"

class CPUARM : public CPU
{
public:
    CPUARM() : CPU() { }
    ~CPUARM(){ }

    void initSMP(Platform *) {}
};
#endif
