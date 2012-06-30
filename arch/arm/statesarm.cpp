#include "statesarm.h"
#include "arm.h"

void StateARM::halt()
{
	m_state = HWStateHalted;
	//hlt();
}

void StateARM::seizeInterrupts()
{
	//cli();
	m_state = HWStateNoInterrupts;
}

void StateARM::startInterrupts()
{
	m_state = HWStateNormal;
	//sti();
}
