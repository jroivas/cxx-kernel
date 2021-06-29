#ifndef __SYSCALL_FILEIO_H
#define  __SYSCALL_FILEIO_H

//#define __NEED_struct_iovec
#define __NEED_size_t
#define __NEED_ssize_t
#define __NEED_locale_t
#include <bits/alltypes.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t syscall_writev(int fd, const struct iovec *iov, int iovcnt);
ssize_t syscall_readv(int fd, const struct iovec *iov, int iovcnt);
int syscall_open(const char *name, int flags, int mode);
int syscall_ioctl(int fd, long cmd, long arg);
int syscall_read(int fd, void *buf, size_t cnt);
int syscall_close(int fd);
int syscall_llseek(int fd, long high, long low, loff_t* res, int orig);
int syscall_fcntl(int fd, int cmd, int arg);
int syscall_fstat(int fd, struct stat *statbuf);

#ifdef __cplusplus
}
#endif

#endif
