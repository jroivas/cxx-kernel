#ifndef TIMERARM_H
#define TIMERARM_H

#include "timer.h"
#include "regs.h"
#include "../platform.h"

class TimerARM : public Timer
{
public:
	TimerARM();
        virtual void setFrequency(unsigned int hz);

protected:
        virtual void run(Regs *r);
	ProcessManager *pm;
};

#endif
