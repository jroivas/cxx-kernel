#include "syscall_arch.h"
#include <stdarg.h>

long internal__syscall0(long n)
{
    (void)n;
    return 0;
}

long internal__syscall1(long n, long a1)
{
    (void)n;
    (void)a1;
    return 0;
}

long internal__syscall2(long n, long a1, long a2)
{
    (void)n;
    (void)a1;
    (void)a2;
    return 0;
}

long internal__syscall3(long n, long a1, long a2, long a3)
{
    (void)n;
    (void)a1;
    (void)a2;
    (void)a3;
    return 0;
}

long internal__syscall4(long n, long a1, long a2, long a3, long a4)
{
    (void)n;
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    return 0;
}

long internal__syscall5(long n, long a1, long a2, long a3, long a4, long a5)
{
    (void)n;
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    return 0;
}

long internal__syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    (void)n;
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    (void)a6;
    return 0;
}

extern "C"
long __syscall_cp_arm(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    (void)n;
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    (void)a6;
    return 0;
}

extern "C"
long syscall(long n, ...)
{
    (void)n;
    va_list ap;
    va_start(ap, n);
    va_end(ap);
    return 0;
}

extern "C"
long __syscall(long n, ...)
{
    (void)n;
    va_list ap;
    va_start(ap, n);
    va_end(ap);
    return 0;
}
