#ifndef CPU_H
#define CPU_H

class Platform;

class CPU
{
public:
    CPU() {}
    virtual ~CPU() { }

    virtual void initSMP(Platform *p) = 0;
};

#endif

