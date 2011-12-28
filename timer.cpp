#include "timer.h"
#include "idt.h"
#include "types.h"
#include "arch/platform.h"

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
        m_ticks = 0;
}

void Timer::setFrequency(unsigned int hz) 
{
	m_hz = hz;
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
        unsigned long target = ticks_to_wait+m_ticks;
        while (m_ticks < target) {
                // Sleep
        }
}

void Timer::sleep(unsigned long sec)
{
	unsigned long t = m_hz*sec;
	wait(t);
}

void Timer::msleep(unsigned long msec)
{
	unsigned long t = msec*m_hz/1000;
	wait(t);
}

void Timer::usleep(unsigned long usec)
{
	unsigned long t = usec*m_hz/10000000;
	wait(t);
}
