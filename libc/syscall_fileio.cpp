#include "syscall_fileio.h"
#include <platform.h>
#include <errno.h>

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

