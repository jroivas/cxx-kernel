#include "platform.h"
#include "types.h"

#ifdef ARCH_x86
#include "x86/statesx86.h"
#include "x86/timerx86.h"
#include "x86/videox86.h"
#include "x86/idtx86.h"
#include "x86/kbx86.h"
#include "x86/vesa.h"
#include "x86/x86.h"
#include "x86/pcix86.h"
#endif

static Timer *__platform_timer = NULL;
static Video *__platform_video = NULL;
static IDT   *__platform_idt   = NULL;
static KB    *__platform_kb    = NULL;
static FB    *__platform_fb    = NULL;
static PCI   *__platform_pci   = NULL;

Platform::Platform()
{
	#ifdef __arm__
	current = PlatformARM;
	m_state = NULL;
	#endif

	#ifdef ARCH_x86
	current = PlatformX86;
	m_state = new StateX86();
	#endif

	#ifdef ARCH_x86_64
	current = PlatformX86_64;
	m_state = NULL;
	#endif
}

Platform::~Platform()
{
	if (m_state!=NULL) delete m_state;
	m_state = NULL;
}

State *Platform::state()
{
	return m_state;
}

int Platform::CAS(ptr_val_t volatile *m_ptr, int cmp, int set)
{
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
}

Timer *Platform::timer()
{
	#ifdef ARCH_x86
	if (__platform_timer==NULL) {
		__platform_timer = new TimerX86();
	}
	#endif

	return __platform_timer;
}

Video *Platform::video()
{
	#ifdef ARCH_x86
	if (__platform_video==NULL) {
		__platform_video = new VideoX86();
	}
	#endif
	return __platform_video;
}

IDT *Platform::idt()
{
	#ifdef ARCH_x86
	if (__platform_idt==NULL) {
		__platform_idt = new IDTX86();
	}
	#endif
	return __platform_idt;
}

KB *Platform::kb()
{
	#ifdef ARCH_x86
	if (__platform_kb==NULL) {
		__platform_kb = new KBX86();
	}
	#endif
	return __platform_kb;
}

FB *Platform::fb()
{
	#ifdef ARCH_x86
	if (__platform_fb==NULL) {
		__platform_fb = new Vesa();
	}
	#endif
	return __platform_fb;
}

PCI *Platform::pci()
{
	#ifdef ARCH_x86
	if (__platform_pci==NULL) {
		__platform_pci = new PCIX86();
	}
	#endif
	return __platform_pci;
}

void Platform::halt()
{
	#ifdef ARCH_x86
	hlt();
	#endif
}

void Platform::seizeInterrupts()
{
	#ifdef ARCH_x86
	cli();
	#endif
}
