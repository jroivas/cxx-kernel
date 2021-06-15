#ifndef _ARCH_ATOMIC_HH
#define _ARCH_ATOMIC_HH

#include "types.h"

static inline void memory_barrier(void) noexcept
{
    __atomic_signal_fence(__ATOMIC_ACQ_REL);
    __atomic_thread_fence(__ATOMIC_ACQ_REL);
}

#ifndef Platform_CAS
inline int Platform_CAS(ptr_val_t volatile *m_ptr, int cmp, int set)
{
    // FIXME implement this properly

    (void)m_ptr;
    (void)cmp;
    (void)set;

    #ifdef ARCH_x86
    if (__sync_bool_compare_and_swap(m_ptr, cmp, set))
        return 1;
#if 0
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
    #endif

    #ifdef ARCH_LINUX
    if ((int)*m_ptr==cmp) {
        *m_ptr = set;
        return 1;
    }
    #endif

    #ifdef ARCH_ARM
    //if ((int)*m_ptr==cmp) {
        *m_ptr = set;
        return 1;
    //}
    #endif

    return 0;
}
#endif

#endif
