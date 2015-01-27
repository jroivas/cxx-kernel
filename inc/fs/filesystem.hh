#ifndef __FILESYSTEM_HH
#define __FILESYSTEM_HH

#include <sys/types.h>
#include <string.hh>
#include <list.hh>

class Dirent
{
public:
    Dirent() {}

    uint64_t m_ino;
    uint64_t m_off;
    unsigned char m_type;

    String m_name;
};

class Dir
{
public:
    Dir(String path)
        : m_path(path),
        m_index(0)
    {
        update();
    }
    virtual ~Dir() {}

    virtual void update();

    virtual void reset();
    virtual Dirent *next();

protected:
    String m_path;
    List m_items;
    uint32_t m_index;
};

class Filesystem
{
public:
    Filesystem() {}
    virtual ~Filesystem() {}

    // File operations
    virtual int create(String path, mode_t mode) = 0;
    virtual int truncate(String path, off_t length) = 0;

    virtual int open(String path, int flags) = 0;

    virtual int chmod(String path, mode_t mode) = 0;
    virtual int getattr(String path, int handle=0) = 0;

    virtual ssize_t read(int fh, char *buf, size_t count) = 0;
    virtual ssize_t write(int fh, const char *buf, size_t count) = 0;

    virtual int symlink(String target, String linkpath) = 0;
    virtual int rename(String oldpath, String newpath) = 0;
    virtual int unlink(String path) = 0;

    virtual int stat(int fd, struct stat *st) = 0;
    virtual int flush(int fd) = 0;

    // Dir operations
    virtual Dir *opendir(String name) = 0;
    virtual Dirent *readdir(Dir *dir) = 0;
    virtual void rewinddir(Dir *dir) = 0;
    virtual int closedir(Dirent *dir) = 0;

    virtual int mkdir(String path, mode_t mode) = 0;
    virtual int rmdir(String path) = 0;
};

#endif
