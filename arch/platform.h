#ifndef PLATFORM_H
#define PLATFORM_H

#include "config.h"

#include "states.h"
#include "timer.h"
#include "video.h"
#include "idt.h"
#include "kb.h"
#include "fb.h"
#include "types.h"
#include "pci.h"
#include "ata.h"
#include "task.h"
#include "processmanager.h"

#ifdef ARCH_x86
#include "x86/pagingx86.h"
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

	static int CAS(ptr_val_t volatile *m_ptr, int cmp, int set);
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
