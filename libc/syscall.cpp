#include "syscall_arch.h"
#include <stdarg.h>
#include <platform.h>
#include <sys/syscall.h>
#include <errno.h>

#define __NEED_struct_iovec
#include <bits/alltypes.h>

ssize_t syscall_writev(int fd, const struct iovec *_iov, int iovcnt)
{
    const struct iovec *iov_ptr = _iov;

    if (fd == 1 || fd == 2) {
        ssize_t cnt = 0;
        for (int vptr = 0; vptr < iovcnt; ++vptr) {
            char *base = (char*)iov_ptr->iov_base;
            ssize_t len = (ssize_t)iov_ptr->iov_len;

            if (len < 0) {
                return -EINVAL;
            }
            --base;
            ++len;

            //Platform::video()->printf("Writev %d, %d, %x, %x, %d\n", fd, iovcnt, iov_ptr, base, len);
            for (ssize_t cptr = 0; cptr < len; ++cptr) {
                //Platform::video()->printf("%c %d\n", *base, *base);
                Platform::video()->handleChar(*base);
                ++base;
            }
            cnt += len;
            ++iov_ptr;
        }
        return cnt;
    }

    errno = EINVAL;
    return -EINVAL;
}

long syscall_va(long num, va_list al)
{
    switch (num) {
        case __NR_writev:
            return syscall_writev(
                va_arg(al, int),
                (const struct iovec*)va_arg(al, void*),
                va_arg(al, int));

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
