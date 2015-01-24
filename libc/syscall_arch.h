#ifndef __SYSCALL_ARCH_H
#define __SYSCALL_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#define __SYSCALL_LL_E(x) \
((union { long long ll; long l[2]; }){ .ll = x }).l[0], \
((union { long long ll; long l[2]; }){ .ll = x }).l[1]
#define __SYSCALL_LL_O(x) __SYSCALL_LL_E((x))

long internal__syscall0(long n);
long internal__syscall1(long n, long a1);
long internal__syscall2(long n, long a1, long a2);
long internal__syscall3(long n, long a1, long a2, long a3);
long internal__syscall4(long n, long a1, long a2, long a3, long a4);
long internal__syscall5(long n, long a1, long a2, long a3, long a4, long a5);
long internal__syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6);

static inline long __syscall0(long n)
{
    return internal__syscall0(n);
}

static inline long __syscall1(long n, long a1)
{
    return internal__syscall1(n, a1);
}

static inline long __syscall2(long n, long a1, long a2)
{
    return internal__syscall2(n, a1, a2);
}

static inline long __syscall3(long n, long a1, long a2, long a3)
{
    return internal__syscall3(n, a1, a2, a3);
}

static inline long __syscall4(long n, long a1, long a2, long a3, long a4)
{
    return internal__syscall4(n, a1, a2, a3, a4);
}

static inline long __syscall5(long n, long a1, long a2, long a3, long a4, long a5)
{
    return internal__syscall5(n, a1, a2, a3, a4, a5);
}

static inline long __syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    return internal__syscall6(n, a1, a2, a3, a4, a5, a6);
}

#if 0
#define VDSO_USEFUL
#define VDSO_CGT_SYM "__vdso_clock_gettime"
#define VDSO_CGT_VER "LINUX_2.6"
#endif

#ifdef __cplusplus
}
#endif

#endif

