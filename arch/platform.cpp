#include "platform.h"
#include "types.h"

#ifdef __i386__
#include "x86/statesx86.h"
#include "x86/timerx86.h"
#include "x86/videox86.h"
#include "x86/idtx86.h"
#include "x86/kbx86.h"
#include "x86/vesa.h"
#include "x86/x86.h"
#endif

static Timer *__platform_timer = NULL;
static Video *__platform_video = NULL;
static IDT   *__platform_idt   = NULL;
static KB    *__platform_kb    = NULL;
static FB    *__platform_fb    = NULL;

Platform::Platform()
{
	#ifdef __arm__
	current = PlatformARM;
	m_state = NULL;
	#endif

	#ifdef __i386__
	current = PlatformX86;
	m_state = new StateX86();
	#endif

	#ifdef __x86_64__
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
	#ifdef __i386__
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

	//if (*m_ptr==cmp)

	*m_ptr=set;
	return 1;
}

Timer *Platform::timer()
{
	#ifdef __i386__
	if (__platform_timer==NULL) {
		__platform_timer = new TimerX86();
	}
	#endif

	return __platform_timer;
}

Video *Platform::video()
{
	#ifdef __i386__
	if (__platform_video==NULL) {
		__platform_video = new VideoX86();
	}
	#endif
	return __platform_video;
}

IDT *Platform::idt()
{
	#ifdef __i386__
	if (__platform_idt==NULL) {
		__platform_idt = new IDTX86();
	}
	#endif
	return __platform_idt;
}

KB *Platform::kb()
{
	#ifdef __i386__
	if (__platform_kb==NULL) {
		__platform_kb = new KBX86();
	}
	#endif
	return __platform_kb;
}

FB *Platform::fb()
{
	#ifdef __i386__
	if (__platform_fb==NULL) {
		__platform_fb = new Vesa();
	}
	#endif
	return __platform_fb;
}

void Platform::halt()
{
	#ifdef __i386__
	hlt();
	#endif
}

void Platform::seizeInterrupts()
{
	#ifdef __i386__
	cli();
	#endif
}
