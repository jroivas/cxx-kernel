#ifndef STATESARM_H
#define STATESARM_H

#include "states.h"

class StateARM : public State
{
public:
	StateARM() : State() { }
	~StateARM(){ }
	void halt();
	void seizeInterrupts();
	void startInterrupts();

};
#endif
