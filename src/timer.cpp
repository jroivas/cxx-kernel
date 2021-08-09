#include "timer.h"
#include "idt.h"
#include "types.h"
#include "arch/platform.h"

static Timer *__global_timer = nullptr;

Timer *Timer::get()
{
    if (__global_timer == nullptr) {
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

int Timer::handler(Regs *r)
{
    if (__global_timer == nullptr) return -1;

    __global_timer->run(r);
    return 0;
}

void Timer::wait(unsigned long ticks_to_wait) const
{
    unsigned long now = m_ticks;
    unsigned long target = ticks_to_wait + now;

    /* Detect overflow */
    if (target < now) {

        /* Wait until ticks flip */
        while (now < m_ticks) {
#ifdef ARCH_x86
            asm("nop");
#endif
        }
    }
    while (m_ticks < target) {
#ifdef ARCH_x86
        asm("nop");
#endif
        //Platform::processManager()->yield();
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
