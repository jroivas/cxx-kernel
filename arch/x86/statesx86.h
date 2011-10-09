#ifndef STATESx86_H
#define STATESx86_H

#include "states.h"

class StateX86 : public State
{
public:
	StateX86() { }
	void halt();
	void seizeInterrupts();
	void startInterrupts();

};
#endif
