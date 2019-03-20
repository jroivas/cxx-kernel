#ifndef CPULINUX_H
#define CPULINUX_H

#include "cpu.h"

class CPULinux : public CPU
{
public:
    CPULinux() : CPU() { }
    ~CPULinux(){ }

    void initSMP() {}
};
#endif

