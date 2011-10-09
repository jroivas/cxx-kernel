#include "timer.h"
#include "idt.h"
#include "types.h"

static Timer *__global_timer = NULL;

Timer *Timer::get()
{
        if (__global_timer==NULL) {
                //__global_timer = new Timer;
                __global_timer = Platform::timer();
        }
        return __global_timer;
}

Timer::Timer()
{
        //IDT::get()->installRoutine(TIMER_ISR_NUMBER, Timer::handler);
        ticks = 0;
}

void Timer::setFrequency(unsigned int hz) 
{
	(void)hz;
}

void Timer::run(Regs *r)
{
	(void)r;
}

void Timer::handler(Regs *r)
{
        if (__global_timer==NULL) return;
        __global_timer->run(r);
}

void Timer::wait(unsigned long ticks_to_wait)
{
        unsigned long target = ticks_to_wait+ticks;
        while (ticks < target) {
                // Sleep
        }
}
