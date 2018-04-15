#include "timerlinux.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "platform.h"

static pthread_t __static_thread;
static unsigned int __static_hz = 1000;

void *__timer_start_routine(void *data)
{
    TimerLinux *p = (TimerLinux*)data;
    (void)p;
    while (1) {
        usleep(__static_hz);
        Timer::handler(nullptr);
    }
    return nullptr;
}

TimerLinux::TimerLinux()
{
    if (pthread_create(&__static_thread, nullptr, __timer_start_routine, this)==0) {
        printf("TimerLinux: thread created\n");
    }
}

void TimerLinux::setFrequency(unsigned int hz)
{
    __static_hz = 10000000 / hz;
    Timer::setFrequency(hz);
}

void TimerLinux::run(Regs *r)
{
    (void)r;
    m_ticks = clock();
}

void TimerLinux::wait(unsigned long ticks_to_wait) const
{
    clock_t target = clock() + ticks_to_wait;
    while (clock() < target) {
        nop();
    }
}
