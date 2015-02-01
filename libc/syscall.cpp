#include "syscall_arch.h"
#include "syscall_fileio.h"
#include <stdarg.h>
#include <platform.h>
#include <sys/syscall.h>
#include <errno.h>

//FIXME Types to be configurable by architecture
long syscall_va(long num, va_list al)
{
    switch (num) {
        case __NR_writev:
            {
                unsigned long fd = va_arg(al, unsigned long);
                const struct iovec *vec = va_arg(al, const struct iovec*);
                unsigned long vlen = va_arg(al, unsigned long);
                return syscall_writev(fd, vec, vlen);
            }
        case __NR_readv:
            {
                unsigned long fd = va_arg(al, unsigned long);
                const struct iovec *vec = va_arg(al, const struct iovec*);
                unsigned long vlen = va_arg(al, unsigned long);
                return syscall_readv(fd, vec, vlen);
            }
        case __NR__llseek:
            {
                unsigned long fd = va_arg(al, unsigned long);
                unsigned long high = va_arg(al, unsigned long);
                unsigned long low = va_arg(al, unsigned long);
                unsigned int orig = va_arg(al, unsigned int);
                loff_t *res = va_arg(al, loff_t*);
                return syscall_llseek(fd, high, low, res, orig);
            }
        case __NR_open:
            {
                const char *fname = va_arg(al, const char*);
                int flags = va_arg(al, int);
                int mode = va_arg(al, int);
                return syscall_open(fname, flags, mode);
            }
        case __NR_ioctl:
            {
                int fd = va_arg(al, int);
                long cmd = va_arg(al, long);
                long arg = va_arg(al, long);
                return syscall_ioctl(fd, cmd, arg);
            }
        case __NR_read:
            {
                int fd = va_arg(al, int);
                void *buf = va_arg(al, void*);
                size_t cnt = va_arg(al, size_t);
                return syscall_read(fd, buf, cnt);
            }
        case __NR_close:
            {
                int fd = va_arg(al, int);
                return syscall_close(fd);
            }

        default:
            Platform::video()->printf("Unsupported syscall %lld\n", num);
            break;
    }
    errno = EINVAL;
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

static struct __ptcb *__cleanup_stack = NULL;

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
