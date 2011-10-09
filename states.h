#ifndef STATES_H
#define STATES_H

class State
{
public:
	State() { m_state = HWStateNormal; }
	virtual void halt() = 0;
	virtual void seizeInterrupts() = 0;
	virtual void startInterrupts() = 0;

	enum HWState {
		HWStateNormal = 0,
		HWStateNoInterrupts = 1,
		HWStateHalted = 2,
	};

	HWState getState() { return m_state; }

protected:
	HWState m_state;

};

#endif
