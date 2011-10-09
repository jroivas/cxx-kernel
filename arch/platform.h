#ifndef PLATFORM_H
#define PLATFORM_H

#include "states.h"

class Platform
{
public:
	Platform();
	~Platform();
	State *state();

private:
	/* Plese extend the platform enums when supported */
	enum Platforms {
		PlatformNone = 0,
		PlatformX86,
		PlatformX86_64,
		PlatformARM
	};
	Platforms current;
	State *m_state;
};

#endif
