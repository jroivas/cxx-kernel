#include "syscall_fileio.h"
#include <platform.h>
#include <fs/filesystem.hh>
#include <sys/ioctl.h>
#include <errno.h>
#include <termios.h>
#include <sys/uio.h>
#include <sys/stat.h>

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

    return -EINVAL;
}

ssize_t syscall_readv(int fd, const struct iovec *iov, int iovcnt)
{
    //Platform::video()->printf(">> readv %d\n", fd);
    if (fd == 1 || fd == 2) {
        return -EPERM;
    }
    if (fd == 0) {
        ssize_t cnt = 0;
        for (int vptr = 0; vptr < iovcnt; ++vptr) {
            size_t len = iov[vptr].iov_len;
            if ((ssize_t)len < 0) {
                return -EINVAL;
            }
            size_t got = 0;
            char *target = (char*)iov[vptr].iov_base;
            while (got < len) {
                if (!Platform::kb()->hasKey()) {
                    Platform::timer()->msleep(10);
                } else {
                    const char *tmp = Platform::kb()->getKey();
                    if (tmp) {
                        if (tmp[0] == '\n')
                            break;
                        int rcnt = String::length(tmp);
                        Mem::copy(target, tmp, rcnt);
                        target += rcnt;
                        got += rcnt;
                    }
                }
            }
            cnt += got;
        }
        return cnt;
        /* TODO read from stdin */
        //return -EBADF;
    }

#ifdef FEATURE_STORAGE
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->accessHandle(fd);
    if (fs == nullptr) {
        return -EPERM;
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
#else
    (void)iov;
    (void)iovcnt;
    return -EPERM;
#endif
}

int syscall_llseek(int fd, long high, long low, loff_t* res, int orig)
{
    (void)fd;
    (void)high;
    (void)low;
    (void)res;
    (void)orig;
    return -EPERM;
}

int syscall_ioctl(int fd, long cmd, long arg)
{
    (void)fd;
    Platform::video()->printf("ioctl %d %d (%d) %x\n", fd, cmd, TCGETS, arg);
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
    if (cmd == TIOCGWINSZ) {
        struct winsize *argp = (struct winsize *)arg;
        // FIXME hardcoded
        argp->ws_col = 80;
        argp->ws_row = 24;
        argp->ws_xpixel = 640;
        argp->ws_ypixel = 480;
        return 0;
    }

    return -EINVAL;
}

int syscall_open(const char *name, int flags, int mode)
{
    //Platform::video()->printf("Opening: %s\n", name);
    (void)mode;
    if (name == nullptr) {
        return -ENOENT;
    }

#ifdef FEATURE_STORAGE
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->access(name);

    if (fs == nullptr) {
        return -ENOENT;
    }

    String base = vfs->stripslash(vfs->basedir(name, fs));
    int res = fs->open(base, flags);

    return res;
#else
    (void)flags;
    return -ENOENT;
#endif
}

int syscall_read(int fd, void *buf, size_t cnt)
{
    if (fd == 0) {
        size_t c = 0;
        Platform::kb()->startReading();
        while (c < cnt) {
            const char *tmp = Platform::kb()->getKey();
            if (!tmp) {
                Platform::timer()->msleep(10);
                continue;
            }
            size_t l = String::length(tmp);
            if (l > cnt)
                l = cnt;
            Mem::copy((char*)buf + c, tmp, l);
            c += l;
            if (tmp[0] == '\n')
                break;
        }
        Platform::kb()->stopReading();
        return c;
    }
#ifdef FEATURE_STORAGE
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->accessHandle(fd);
    if (fs == nullptr) {
        return -EPERM;
    }

    int res = fs->read(fd, (char*)buf, cnt);
    return res;
#else
    (void)fd;
    (void)buf;
    (void)cnt;
    return -ENOENT;
#endif
}

int syscall_write(int fd, void *buf, size_t cnt)
{
    if (fd == 1 || fd == 2) {

        for (size_t i = 0; i < cnt; i++)
            Platform::video()->handleChar(((char*)buf)[i]);
        return cnt;
    }
#ifdef FEATURE_STORAGE
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->accessHandle(fd);
    if (fs == nullptr) {
        return -EPERM;
    }

    return fs->write(fd, (char*)buf, cnt);
#else
    (void)fd;
    (void)buf;
    (void)cnt;
    return -ENOENT;
#endif
}

int syscall_close(int fd)
{
#ifdef FEATURE_STORAGE
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->accessHandle(fd);
    if (fs == nullptr) {
        return -EPERM;
    }

    return fs->close(fd);
#else
    (void)fd;
    return -ENOENT;
#endif
}

int syscall_fcntl(int fd, int cmd, int arg)
{
    //Platform::video()->printf("fcntl64 on %d %d %d\n", fd, cmd, arg);
#ifdef FEATURE_STORAGE
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->accessHandle(fd);
    if (fs == nullptr) {
        return -EPERM;
    }

    if (cmd == F_SETFD) {
        return fs->setFcntl(fd, arg);
    }
    else if (cmd == F_GETFD) {
        return fs->getFcntl(fd);
    }
#else
    (void)fd;
    (void)cmd;
    (void)arg;
#endif
    Platform::video()->printf("fcntl64 err\n");
    return -ENOENT;
}

int syscall_fstat(int fd, struct stat *sb)
{
    //Platform::video()->printf("Called: fstat\n");
    if (sb == nullptr) {
        return -EINVAL;
    }

    /* Pseudo stdin/stdout/stderr devices, fake the data */
    if (fd >= 0 && fd <= 2) {
        Mem::set(sb, 0, sizeof(struct stat));

        sb->st_dev = 24;
        sb->st_ino = 13;
        sb->st_mode = 020620;
        sb->st_nlink = 1;
        sb->st_uid = 0;
        sb->st_rdev = 34826;
        sb->st_size = 0;
        sb->st_blksize = 1024;
        sb->st_blocks = 0;

        return 0;
    }
    if (fd >= 3) {
#ifdef FEATURE_STORAGE
        VFS *vfs = Platform::vfs();
        Filesystem *fs = vfs->accessHandle(fd);
        if (!fs)
            return -EPERM;

        Mem::set(sb, 0, sizeof(struct stat));

        // FIXME set proper values
        sb->st_dev = fs->getDevId();
        sb->st_mode = 020600;
        sb->st_nlink = 1;
        sb->st_rdev = 34826;

        return fs->stat(fd, sb);
#endif

#if 0
        sb->st_dev = 99;
        sb->st_ino = 99;
        sb->st_mode = 020620;
        sb->st_nlink = 1;
        sb->st_uid = 0;
        sb->st_rdev = 34826;
        sb->st_size = fs->;
        sb->st_blksize = 1024;
        sb->st_blocks = 0;

        return 0;
#endif
    }
#ifdef FEATURE_STORAGE
#endif
    Platform::video()->printf("fstat failed\n");
    return -ENOENT;
}

int syscall_stat(const char *pathname, struct stat *statbuf)
{
    (void)statbuf;
    if (pathname == nullptr) {
        return -ENOENT;
    }
    Platform::video()->printf("stat: %s\n", pathname);
    // FIXME hardcoded
    if (String("/proc/self/fd/3") == pathname) {
        int res;
        res = syscall_fstat(3, statbuf);
        Platform::video()->printf("statres: %d\n", res);
        return res;
#if 0
        Mem::set(statbuf, 0, sizeof(*statbuf));
        Platform::video()->printf("fk\n");
        statbuf->st_mode = 0700;
        statbuf->st_nlink = 1;
        return 0;
#endif
    }

#ifdef FEATURE_STORAGE
    VFS *vfs = Platform::vfs();
    Filesystem *fs = vfs->access(pathname);

    if (fs == nullptr) {
        return -ENOENT;
    }

    String base = vfs->stripslash(vfs->basedir(pathname, fs));
    int fd = fs->open(base, O_RDONLY);
    if (fd < 0) {
        return -ENOENT;
    }

    int res = fs->stat(fd, statbuf);
    if (res < 0)
        return -EBADF;
    return res;
#endif

    return -ENOENT;
}

int syscall_readlink(const char *pathname, char *buf, size_t bufsize)
{
    (void)pathname;
    (void)buf;
    (void)bufsize;

    if (pathname == nullptr || String::length(pathname) == 0) {
        return -ENOENT;
    }

    // FIXME hardcoding
#if 0
    if (String("/proc/self/fd/3") == pathname) {
        Mem::copy(buf, "/proc/42/3", 10);
        return 10;
    }
#endif
#if 1
    Platform::video()->printf("Readlink: %s\n", pathname);
    size_t cnt = 0;
    const char *r = pathname;
    char *d = buf;
    while (r && *r && cnt < bufsize) {
        *d++ = *r++;
        cnt++;
    }
    Platform::video()->printf("Readlink got: %s, %d\n", buf, cnt);
#endif

    return cnt;
}

long syscall_getcwd(char *buf, size_t size)
{
    if (buf == nullptr || size == 0) {
        return -EINVAL;
    }

    if (size < 2) {
        return -ENAMETOOLONG;
    }

    buf[0] = '/';
    buf[1] = 0;

    return 0;
}
