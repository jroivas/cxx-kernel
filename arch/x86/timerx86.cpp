#include "timerx86.h"
#include "port.h"
#include "idtx86.h"
#include "task.h"

#define TIMER_ISR_NUMBER 0

TimerX86::TimerX86() : Timer()
{
	IDT::get()->installRoutine(TIMER_ISR_NUMBER, TimerX86::handler);
	pm = Platform::processManager();
}

void TimerX86::setFrequency(unsigned int hz)
{
        unsigned int val = 1193180/hz;
        Port::out(0x43, 0x36);
        Port::out(0x40, val&0xFF);
        Port::out(0x40, (val>>8)&0xFF);
	Timer::setFrequency(hz);
}

void TimerX86::run(Regs *r)
{
        (void)r;
        m_ticks++;

	//if (m_ticks%10==0) Platform::video()->printf("T");

	if (pm!=NULL && pm->isRunning()) {
		if (pm->hasSlice()) pm->decSlice();
		else {
			Port::out(0x20, 0x20);
			pm->schedule();
#if 0
			Task *t = pm->schedule();
			if (t!=NULL) {
				//t->switchTo();
			}
#endif
		}
	}
        //Port::out(0x20, 0x20);
	//if (m_ticks%10==0) Platform::video()->printf("t");
}
