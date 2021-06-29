#include "syscall_arch.h"
#include "syscall_fileio.h"
#include "syscall_mem.h"
#include <stdarg.h>
#include <platform.h>
#include <sys/syscall.h>
#include <errno.h>

#define SYS_CALL0(X, Y) \
    case X: return Y();
#define SYS_CALL1(X, Y, at) \
    case X: {\
        at a = va_arg(al, at);\
        return Y(a);\
    }
#define SYS_CALL2(X, Y, at, bt) \
    case X: {\
        at a = va_arg(al, at);\
        bt b = va_arg(al, bt);\
        return Y(a, b);\
    }
#define SYS_CALL3(X, Y, at, bt, ct) \
    case X: {\
        at a = va_arg(al, at);\
        bt b = va_arg(al, bt);\
        ct c = va_arg(al, ct);\
        return Y(a, b, c);\
    }
#define SYS_CALL4(X, Y, at, bt, ct, dt) \
    case X: {\
        at a = va_arg(al, at);\
        bt b = va_arg(al, bt);\
        ct c = va_arg(al, ct);\
        dt d = va_arg(al, dt);\
        return Y(a, b, c, d);\
    }
#define SYS_CALL5(X, Y, at, bt, ct, dt, et) \
    case X: {\
        at a = va_arg(al, at);\
        bt b = va_arg(al, bt);\
        ct c = va_arg(al, ct);\
        dt d = va_arg(al, dt);\
        et e = va_arg(al, et);\
        return Y(a, b, c, d, e);\
    }
#define SYS_CALL6(X, Y, at, bt, ct, dt, et, ft) \
    case X: {\
        at a = va_arg(al, at);\
        bt b = va_arg(al, bt);\
        ct c = va_arg(al, ct);\
        dt d = va_arg(al, dt);\
        et e = va_arg(al, et);\
        ft f = va_arg(al, ft);\
        return Y(a, b, c, d, e, f);\
    }

//FIXME Types to be configurable by architecture
long syscall_va(long num, va_list al)
{
    //Platform::video()->printf("syscall %d\n", num);
    switch (num) {
        SYS_CALL3(SYS_writev, syscall_writev, unsigned long, struct iovec*, unsigned long);
        SYS_CALL3(SYS_readv, syscall_readv, unsigned long, struct iovec*, unsigned long);
        SYS_CALL5(SYS__llseek, syscall_llseek, unsigned long, unsigned long, unsigned long, loff_t *, unsigned int);
        SYS_CALL3(SYS_open, syscall_open, const char*, int, int);
        SYS_CALL3(SYS_ioctl, syscall_ioctl, int, long, long);
        SYS_CALL3(SYS_read, syscall_read, int, void *, size_t);
        SYS_CALL1(SYS_close, syscall_close, int);
        SYS_CALL3(SYS_fcntl64, syscall_fcntl, int, int, int);
        SYS_CALL6(SYS_mmap2, syscall_mmap_wrap, void *, size_t, int, int, int, off_t);
        SYS_CALL2(SYS_fstat64, syscall_fstat, int, struct stat*);
        default:
            Platform::video()->printf("Unsupported syscall %lld\n", num);
            break;
    }

    errno = EINVAL;
    Platform::video()->printf("*** ERROR: Unimplemented syscall: %ld\n", num);
    while(1) {}
    return -EINVAL;
}

extern "C"
long syscall(long n, ...)
{
    va_list ap;
    va_start(ap, n);

    long res = syscall_va(n, ap);

    va_end(ap);
    return res;
}

extern "C"
long __syscall(long n, ...)
{
    va_list ap;
    va_start(ap, n);

    long res = syscall_va(n, ap);

    va_end(ap);
    return res;
}

long internal__syscall0(long n)
{
    return __syscall(n);
}

long internal__syscall1(long n, long a1)
{
    return __syscall(n, a1);
}

long internal__syscall2(long n, long a1, long a2)
{
    return __syscall(n, a1, a2);
}

long internal__syscall3(long n, long a1, long a2, long a3)
{
    return __syscall(n, a1, a2, a3);
}

long internal__syscall4(long n, long a1, long a2, long a3, long a4)
{
    return __syscall(n, a1, a2, a3, a4);
}

long internal__syscall5(long n, long a1, long a2, long a3, long a4, long a5)
{
    return __syscall(n, a1, a2, a3, a4, a5);
}

long internal__syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    return __syscall(n, a1, a2, a3, a4, a5, a6);
}

extern "C"
long __syscall_cp_asm(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    return __syscall(n, a1, a2, a3, a4, a5, a6);
}

#if 0
extern "C"
long __syscall_cp(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    return __syscall(n, a1, a2, a3, a4, a5, a6);
}

struct __ptcb {
    void (*__f)(void *);
    void *__x;
    struct __ptcb *__next;
};

static struct __ptcb *__cleanup_stack = nullptr;

extern "C"
void _pthread_cleanup_push(struct __ptcb *p, void (*f)(void *), void *x)
{
    p->__f = f;
    p->__x = x;
    p->__next = __cleanup_stack;
    __cleanup_stack = p;
}

extern "C"
void _pthread_cleanup_pop(struct __ptcb *p, int r)
{
    __cleanup_stack = p->__next;
    if (r) p->__f(p->__x);
}

extern "C"
void __lock(volatile int *l)
{
    //FIXME
    (void)l;
}

extern "C"
void __unlock(volatile int *l)
{
    //FIXME
    (void)l;
}

extern "C"
void __wait(volatile int *addr, volatile int *waiters, int val, int priv)
{
    (void)addr;
    (void)waiters;
    (void)val;
    (void)priv;
}
#endif
