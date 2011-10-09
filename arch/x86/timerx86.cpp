#include "timerx86.h"
#include "port.h"
#include "idtx86.h"

#define TIMER_ISR_NUMBER 0

TimerX86::TimerX86()
{
	IDT::get()->installRoutine(TIMER_ISR_NUMBER, TimerX86::handler);
}

void TimerX86::setFrequency(unsigned int hz)
{
        unsigned int val = 1193180/hz;
        Port::out(0x43, 0x36);
        Port::out(0x40, val&0xFF);
        Port::out(0x40, (val>>8)&0xFF);
}

void TimerX86::run(Regs *r)
{
        (void)r;
        ticks++;
        if (ticks%10==0) {
                Port::out(0x20, 0x20);
        }
}
