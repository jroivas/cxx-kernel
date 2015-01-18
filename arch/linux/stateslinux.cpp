#include "stateslinux.h"

void StateLinux::halt()
{
    m_state = HWStateHalted;
    while(1);
}

void StateLinux::seizeInterrupts()
{
    //FIXME
    m_state = HWStateNoInterrupts;
}

void StateLinux::startInterrupts()
{
    //FIXME
    m_state = HWStateNormal;
}
