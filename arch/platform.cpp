#include "platform.h"
#include "types.h"

#ifdef __i386__
#include "x86/statesx86.h"
#include "x86/timerx86.h"
#include "x86/videox86.h"
#endif

static Timer *__platform_timer = NULL;
static Video *__platform_video = NULL;

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
