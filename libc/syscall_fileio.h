#ifndef __SYSCALL_FILEIO_H
#define  __SYSCALL_FILEIO_H

#define __NEED_struct_iovec
#define __NEED_size_t
#define __NEED_ssize_t
#include <bits/alltypes.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t syscall_writev(int fd, const struct iovec *_iov, int iovcnt);
int syscall_open(const char *name, int flags, int mode);
int syscall_ioctl(int fd, long cmd, long arg);
int syscall_read(int fd, void *buf, size_t cnt);
int syscall_close(int fd);

#ifdef __cplusplus
}
#endif

#endif
