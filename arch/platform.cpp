#include "platform.h"
#include "types.h"
#ifdef __i386__
#include "x86/statesx86.h"
#endif

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
