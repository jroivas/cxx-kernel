#include <fs/devfs.hh>
#include <errno.h>
#include <fcntl.h>

Filesystem *DevFS::mount(String mountpoint, String options)
{
    DevFS *tmp = new DevFS();
    tmp->m_mountpoint = mountpoint;
    tmp->m_opts = options;
    return tmp;
}

bool DevFS::umount(Filesystem *fs)
{
    DevFS *tmp = (DevFS*)fs;
    delete tmp;

    return true;
}

int DevFS::truncate(String path, off_t length)
{
    (void)path;
    (void)length;
    errno = EPERM;
    return -1;
}

int DevFS::open(String path, int flags)
{
    if ((flags & O_APPEND)
        || (flags & O_CREAT)
        || (flags & O_TRUNC)
        || (flags & O_TMPFILE)
        || (flags & O_WRONLY)
        || (flags & O_RDWR)) {
        errno = EPERM;
        return -1;
    }
    //FIXME
    if (path == "random") {
        return mapfile(type(), "random");
    }
    errno = ENOENT;
    return -1;
}

int DevFS::close(int fh)
{
    if (!closefile(fh)) {
        errno = EBADF;
        return -1;
    }
    return 0;
}

int DevFS::chmod(String path, mode_t mode)
{
    (void)path;
    (void)mode;
    errno = EPERM;
    return -1;
}

int DevFS::getattr(String path, int handle)
{
    (void)path;
    (void)handle;
    errno = EPERM;
    return -1;
}

ssize_t DevFS::read(int fh, char *buf, size_t count)
{
    (void)fh;
    (void)buf;
    (void)count;
    return 0;
}

ssize_t DevFS::write(int fh, const char *buf, size_t count)
{
    (void)fh;
    (void)buf;
    (void)count;
    errno = EPERM;
    return -1;
}

int DevFS::symlink(String target, String linkpath)
{
    (void)target;
    (void)linkpath;
    errno = EPERM;
    return -1;
}

int DevFS::rename(String oldpath, String newpath)
{
    (void)oldpath;
    (void)newpath;
    errno = EPERM;
    return -1;
}

int DevFS::unlink(String path)
{
    (void)path;
    errno = EPERM;
    return -1;
}

int DevFS::stat(int fd, struct stat *st)
{
    (void)fd;
    (void)st;
    return -1;
}

int DevFS::flush(int fd)
{
    (void)fd;
    return 0;
}

Dir *DevFS::opendir(String name)
{
    (void)name;
    return NULL;
}

Dirent *DevFS::readdir(Dir *dir)
{
    (void)dir;
    return NULL;
}

void DevFS::rewinddir(Dir *dir)
{
    (void)dir;
}

int DevFS::closedir(Dirent *dir)
{
    (void)dir;
    return -1;
}

int DevFS::mkdir(String path, mode_t mode)
{
    (void)path;
    (void)mode;
    errno = EPERM;
    return -1;
}

int DevFS::rmdir(String path)
{
    (void)path;
    errno = EPERM;
    return -1;
}
