#ifndef STATES_LINUX_H
#define STATES_LINUX_H

#include "states.h"

class StateLinux : public State
{
public:
	StateLinux() : State() { }
	~StateLinux(){ }
	void halt();
	void seizeInterrupts();
	void startInterrupts();

};
#endif
