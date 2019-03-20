#ifndef CPU_H
#define CPU_H

class CPU
{
public:
    CPU() {}
    virtual ~CPU() { }

    void initSMP();
};

#endif

