#ifndef _ATOMIC_H
#define _ATOMIC_H

#include "config.h"
#include "types.h"

#if 0
#ifdef ARCH_x86
#define Platform_CAS(__cas_m_ptr,__cas_cmp,__cas_set) \
	int __cas_res = __cas_cmp;\
	asm volatile( "lock; cmpxchgl %1,%2\n" "setz %%al\n" "movzbl %%al,%0" : "+a"(__cas_res) : "r" (__cas_set), "m"(*(__cas_m_ptr)) : "memory") 
#endif
#endif

#ifndef Platform_CAS
inline int Platform_CAS(ptr_val_t volatile *m_ptr, int cmp, int set) {
	// FIXME implement this properly

	(void)m_ptr;
	(void)cmp;
	(void)set;

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

	#ifdef ARCH_LINUX
	if ((int)*m_ptr==cmp) {
		*m_ptr=set;
		return 1;
	}
	#endif
	return 0;
}
#endif
#endif
