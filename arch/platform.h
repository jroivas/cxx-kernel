#ifndef PLATFORM_H
#define PLATFORM_H

#include "config.h"
#include "types.h"

#include "states.h"
#include "timer.h"
#include "video.h"
#include "idt.h"
#include "kb.h"
#include "fb.h"
#include "pci.h"
#include "ata.h"
#include "task.h"
#include "processmanager.h"
#include "atomic.h"

#ifdef ARCH_x86
#include "x86/pagingx86.h"
#endif
#ifdef ARCH_ARM
#include "arm/pagingarm.h"
#endif
#ifdef ARCH_LINUX
#include "linux/paginglinux.h"
#endif

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
	static FB *fb();
	static PCI *pci();
	static ATA *ata();
	static Task *task();
	static ProcessManager *processManager();

	static inline int CAS(ptr_val_t volatile *m_ptr, int cmp, int set) {
		return Platform_CAS(m_ptr, cmp, set);
#if 0
		// FIXME implement this properly

		#ifdef ARCH_x86
		int res = cmp;
		asm volatile(
			"lock; cmpxchgl %1,%2\n"
			"setz %%al\n"
			"movzbl %%al,%0"
			: "+a"(res)
			: "r" (set), "m"(*(m_ptr))
			: "memory"
			);

		return res;
		#endif

		if ((int)*m_ptr==cmp) {
			*m_ptr=set;
			return 1;
		}
		return 0;
#endif
	}
	static void halt();
	static void seizeInterrupts();
	static void continueInterrupts();

private:
	/* Plese extend the platform enums when supported */
	enum Platforms {
		PlatformNone = 0,
		PlatformX86,
		PlatformX86_64,
		PlatformARM,
		PlatformLinux
	};
	Platforms current;
	State *m_state;
};

#endif
