#include "syscall_fileio.h"
#include <platform.h>
#include <errno.h>
#include <fs/filesystem.hh>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/uio.h>

ssize_t syscall_writev(int fd, const struct iovec *iov, int iovcnt)
{
    if (fd == 1 || fd == 2) {
        ssize_t cnt = 0;
        for (int vptr = 0; vptr < iovcnt; ++vptr) {
            ssize_t len = iov[vptr].iov_len;

            if (len < 0) {
                return -EINVAL;
            }

            for (ssize_t cptr = 0; cptr < len; ++cptr) {
                Platform::video()->handleChar(((char*)iov[vptr].iov_base)[cptr]);
            }
            cnt += len;
        }
        return cnt;
    }

    errno = EINVAL;
    return -EINVAL;
}

ssize_t syscall_readv(int fd, const struct iovec *iov, int iovcnt)
{
    if (fd == 1 || fd == 2) {
        errno = EPERM;
        return -1;
    }

    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->accessHandle(fd);
    if (fs == NULL) {
        errno = EPERM;
        return -1;
    }

    ssize_t cnt = 0;
    for (int vptr = 0; vptr < iovcnt; ++vptr) {
        size_t len = iov[vptr].iov_len;
        if ((ssize_t)len < 0) {
            return -EINVAL;
        }
        size_t got = 0;
        char *target = (char*)iov[vptr].iov_base;
        while (got < len) {
            size_t rcnt = fs->read(fd, target, len - got);
            if (rcnt <= 0) break;
            target += rcnt;
            got += rcnt;
        }
        cnt += got;
    }
    return cnt;
}

int syscall_llseek(int fd, long high, long low, loff_t* res, int orig)
{
    (void)fd;
    (void)high;
    (void)low;
    (void)res;
    (void)orig;
    errno = EPERM;
    return -1;
}

int syscall_ioctl(int fd, long cmd, long arg)
{
    (void)fd;
    if (cmd == TCGETS) {
        struct termios *argp = (struct termios *)arg;

        argp->c_iflag = IGNPAR;
        argp->c_oflag = OPOST | ONOCR | ONLRET;
        argp->c_cflag = 0;
        argp->c_lflag = 0;
        for (int i = 0; i < NCCS; ++i) {
            argp->c_cc[i] = 0;
        }

        return 0;
    }

    errno = EINVAL;
    return -1;
}

int syscall_open(const char *name, int flags, int mode)
{
    (void)mode;
    if (name == NULL) {
        errno = ENOENT;
        return -1;
    }

    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->access(name);

    if (fs == NULL) {
        errno = ENOENT;
        return -1;
    }

    String base = vfs->stripslash(vfs->basedir(name, fs));
    return fs->open(base, flags);
}

int syscall_read(int fd, void *buf, size_t cnt)
{
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->accessHandle(fd);
    if (fs == NULL) {
        errno = EPERM;
        return -1;
    }

    return fs->read(fd, (char*)buf, cnt);
}

int syscall_close(int fd)
{
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->accessHandle(fd);
    if (fs == NULL) {
        errno = EPERM;
        return -1;
    }

    return fs->close(fd);
}
