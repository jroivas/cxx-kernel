#include "timerx86.h"
#include "port.h"
#include "idtx86.h"
#include "task.h"

#define TIMER_ISR_NUMBER 32

TimerX86::TimerX86() : Timer()
{
    IDT::get()->installRoutine(TIMER_ISR_NUMBER, TimerX86::handler);
    pm = Platform::processManager();
}

void TimerX86::setFrequency(unsigned int hz)
{
    if (hz == 0) ++hz;

    unsigned int val = 1193180 / hz;
    Port::out(0x43, 0x36);
    Port::out(0x40, val & 0xFF);
    Port::out(0x40, (val >> 8) & 0xFF);
    Timer::setFrequency(hz);
}

void TimerX86::run(Regs *r)
{
    (void)r;
    ++m_ticks;

    if (pm != NULL && pm->isRunning()) {
        if (pm->hasSlice()) {
            pm->decSlice();
        } else {
            pm->schedule();
            Port::out(0x20, 0x20);
        }
    }
}

void TimerX86::nop() const
{
    asm("nop");
}
