#ifndef TIMER_LINUX_H
#define TIMER_LINUX_H

#include "timer.h"
#include "regs.h"

class TimerLinux : public Timer
{
public:
	TimerLinux();
        virtual void setFrequency(unsigned int hz);

protected:
        virtual void run(Regs *r);

private:
};

#endif
