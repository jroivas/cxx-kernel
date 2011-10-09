#ifndef TIMER_H
#define TIMER_H

#include "regs.h"

class Timer
{
public:
        static Timer *get();
        void wait(unsigned long ticks_to_wait);
        unsigned long getTicks() { return ticks; }
        virtual void setFrequency(unsigned int hz);

protected:
        Timer();
        virtual void run(Regs *r);
        static void handler(Regs *r);
        unsigned long ticks;
};

#endif
