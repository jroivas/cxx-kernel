#include "timer.h"
#include "idt.h"
#include "port.h"
#include "types.h"

static Timer *__global_timer = NULL;

Timer *Timer::getInstance()
{
        if (__global_timer==NULL) {
                __global_timer = new Timer;
        }
        return __global_timer;
}

Timer::Timer()
{
        IDT::getInstance()->installRoutine(0, Timer::handler);
        ticks = 0;
}

void Timer::setFrequency(unsigned int hz)
{
        unsigned int val = 1193180/hz;
        Port::out(0x43, 0x36);
        Port::out(0x40, val&0xFF);
        Port::out(0x40, (val>>8)&0xFF);
}

void Timer::run(Regs *r)
{
        (void)r;
        ticks++;
        if (ticks%10==0) {
                Port::out(0x20, 0x20);
        }
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
