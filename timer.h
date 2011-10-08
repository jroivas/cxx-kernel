#ifndef TIMER_H
#define TIMER_H

#include "regs.h"

class Timer
{
public:
        static Timer *getInstance();
        void setFrequency(unsigned int hz);
        void wait(unsigned long ticks_to_wait);
        unsigned long getTicks() { return ticks; }
        static void handler(Regs *r);

private:
        Timer();
        void run(Regs *r);
        unsigned long ticks;
};

#endif
