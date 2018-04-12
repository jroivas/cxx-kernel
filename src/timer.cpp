#include "timer.h"
#include "idt.h"
#include "types.h"
#include "arch/platform.h"

static Timer *__global_timer = NULL;

Timer *Timer::get()
{
    if (__global_timer == NULL) {
        __global_timer = Platform::timer();
    }
    return __global_timer;
}

Timer::Timer()
{
    m_ticks = 0;
}

void Timer::setFrequency(unsigned int hz)
{
    m_hz = hz;
}

void Timer::run(Regs *r)
{
    (void)r;
    //TODO
}

void Timer::nop() const
{
}

int Timer::handler(Regs *r)
{
    if (__global_timer == NULL) return -1;

    __global_timer->run(r);
    return 0;
}

void Timer::wait(unsigned long ticks_to_wait) const
{
    unsigned long target = ticks_to_wait + m_ticks;
    while (m_ticks < target) {
        nop();
        // TODO: Sleep
    }
}

void Timer::sleep(unsigned long sec) const
{
    unsigned long t = m_hz * sec;
    wait(t);
}

void Timer::msleep(unsigned long msec) const
{
    unsigned long t = msec * m_hz / 1000;
    wait(t);
}

void Timer::usleep(unsigned long usec) const
{
    unsigned long t = usec * m_hz / 10000000;
    wait(t);
}
