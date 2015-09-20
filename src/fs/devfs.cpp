#include <fs/devfs.hh>
#include <errno.h>
#include <fcntl.h>
#include <platform.h>

DevFS::DevFS(const DevFS &fs)
{
    m_mountpoint = fs.m_mountpoint;
    m_opts = fs.m_mountpoint;
}

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
    if (path == "random"
        || path == "urandom") {
        int res = mapfile(type(), path, this, NULL);
        return res;
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

static unsigned long int rand_seed = 1;
static bool srand_called = false;

static void srand(unsigned int seed)
{
    rand_seed = seed;
}

static int rand(void)
{
    if (!srand_called) {
        srand(Platform::timer()->getTicks());
        srand_called = true;
    }
    rand_seed = rand_seed * 1103515245 + 12345;
    return (unsigned int)(rand_seed / 65536) % 32768;
}

ssize_t DevFS::read(int fh, char *buf, size_t count)
{
    String name = getName(fh);
    if (name == "random"
        || name == "urandom") {
        ssize_t cnt = 0;
        for (cnt = 0; cnt < (ssize_t)count; ++cnt) {
            *buf = (rand() % 256) & 0xFF;
            ++buf;
        }
        return cnt;
    }

    errno = ENOENT;
    return -1;
}

int DevFS::fseek(
    int fd, long offs_hi, long offs_low,
    loff_t *result, unsigned int orig)
{
    String name = getName(fd);
    if (name == "random"
        || name == "urandom") {
        return 0;
    }

    (void)offs_hi;
    (void)offs_low;
    (void)result;
    (void)orig;

    errno = EBADF;
    return -1;
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
