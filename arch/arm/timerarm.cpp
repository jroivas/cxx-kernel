#include "timerarm.h"

//#define TIMER_ISR_NUMBER 0
//#define TIMER_ISR_NUMBER 32

TimerARM::TimerARM() : Timer()
{
/*
	IDT::get()->installRoutine(TIMER_ISR_NUMBER, TimerARM::handler);
	pm = Platform::processManager();
*/
}

void TimerARM::setFrequency(unsigned int hz)
{
/*
        unsigned int val = 1193180/hz;
        Port::out(0x43, 0x36);
        Port::out(0x40, val&0xFF);
        Port::out(0x40, (val>>8)&0xFF);
*/
	Timer::setFrequency(hz);
}

void TimerARM::run(Regs *r)
{
        (void)r;
        m_ticks++;
#if 0
        (void)r;
        m_ticks++;

	//if (m_ticks%10==0) Platform::video()->printf("T");

	if (pm!=NULL && pm->isRunning()) {
		if (pm->hasSlice()) pm->decSlice();
		else {
			pm->schedule();
			Port::out(0x20, 0x20);
			Task *t = pm->schedule();
			if (t!=NULL) {
				//t->switchTo();
			}
		}
	}
        //Port::out(0x20, 0x20);
	//if (m_ticks%10==0) Platform::video()->printf("t");
#endif
}
