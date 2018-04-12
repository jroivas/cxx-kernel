#ifndef TIMERX86_H
#define TIMERX86_H

#include "timer.h"
#include "regs.h"
#include "platform.h"

class TimerX86 : public Timer
{
public:
    TimerX86();
    virtual void setFrequency(unsigned int hz);

protected:
    virtual void run(Regs *r);
    virtual void nop() const;
    ProcessManager *pm;
};

#endif
