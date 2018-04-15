#ifndef TIMER_LINUX_H
#define TIMER_LINUX_H

#include "timer.h"
#include "regs.h"

class TimerLinux : public Timer
{
public:
    TimerLinux();
    virtual void setFrequency(unsigned int hz);
    virtual void wait(unsigned long ticks_to_wait) const;

protected:
    virtual void run(Regs *r);
};

#endif
