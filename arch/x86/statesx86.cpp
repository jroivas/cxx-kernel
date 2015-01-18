#include "statesx86.h"
#include "x86.h"

void StateX86::halt()
{
    m_state = HWStateHalted;
    hlt();
}

void StateX86::seizeInterrupts()
{
    cli();
    m_state = HWStateNoInterrupts;
}

void StateX86::startInterrupts()
{
    m_state = HWStateNormal;
    sti();
}
