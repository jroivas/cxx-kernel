#ifndef PLATFORM_H
#define PLATFORM_H

#include "states.h"
#include "timer.h"
#include "video.h"
#include "idt.h"
#include "kb.h"
#include "types.h"

class Platform
{
public:
	Platform();
	~Platform();
	State *state();
	static Timer *timer();
	static Video *video();
	static IDT *idt();
	static KB *kb();
	static int CAS(ptr_val_t volatile *m_ptr, int cmp, int set);

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
