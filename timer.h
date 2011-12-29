#ifndef TIMER_H
#define TIMER_H

#include "regs.h"

class Timer
{
public:
        static Timer *get();
        void wait(unsigned long ticks_to_wait);
        unsigned long getTicks() { return m_ticks; }
        virtual void setFrequency(unsigned int hz);
	void sleep(unsigned long sec);
	void msleep(unsigned long msec);
	void usleep(unsigned long usec);
        static void handler(Regs *r);

protected:
        Timer();
        virtual void run(Regs *r);
        unsigned long m_ticks;
	unsigned long m_hz;
};

#endif
