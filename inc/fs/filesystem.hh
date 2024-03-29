#ifndef __FILESYSTEM_HH
#define __FILESYSTEM_HH

#include <fcntl.h>
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

class FilesystemPhys
{
public:
    virtual bool read(
        uint8_t *buffer,
        uint32_t sectors,
        uint32_t pos,
        uint32_t pos_hi) = 0;
    virtual bool write(
        uint8_t *buffer,
        uint32_t sectors,
        uint32_t pos,
        uint32_t pos_hi) = 0;

    virtual uint64_t size() const = 0;
    virtual uint32_t sectorSize() const = 0;
};

class Filesystem
{
public:
    Filesystem() {}
    virtual ~Filesystem() {}

    virtual Filesystem *mount(String mountpoint, String options) = 0;
    virtual void setPhysical(FilesystemPhys *phys) = 0;
    virtual uint32_t getDevId() const
    {
        return 0;
    }

    virtual bool umount(Filesystem *fs) = 0;
    virtual const String &mountpoint() const = 0;

    virtual const String type() const = 0;

    // File operations
    virtual int truncate(String path, off_t length) = 0;

    virtual int open(String path, int flags) = 0;
    virtual int close(int fh) = 0;

    virtual int chmod(String path, mode_t mode) = 0;
    virtual int getattr(String path, int handle=0) = 0;

    virtual ssize_t read(int fh, char *buf, size_t count) = 0;
    virtual ssize_t write(int fh, const char *buf, size_t count) = 0;

    virtual int symlink(String target, String linkpath) = 0;
    virtual int rename(String oldpath, String newpath) = 0;
    virtual int unlink(String path) = 0;

    virtual int stat(int fd, struct stat *st) = 0;
    virtual int flush(int fd) = 0;

    virtual int fseek(
        int fd, long offs_hi, long offs_low,
        loff_t *result, unsigned int orig) = 0;

    // Dir operations
    virtual Dir *opendir(String name) = 0;
    virtual Dirent *readdir(Dir *dir) = 0;
    virtual void rewinddir(Dir *dir) = 0;
    virtual int closedir(Dirent *dir) = 0;

    virtual int mkdir(String path, mode_t mode) = 0;
    virtual int rmdir(String path) = 0;

    Filesystem *getFilesystem(int fh);

    int getFcntl(int fh);
    int setFcntl(int fh, int mode);

protected:
    int mapfile(
        const String &fs,
        const String &name,
        Filesystem *owner,
        void *custom);
    int getfile(const String &fs, const String &name) const;
    const String getFS(int fh) const;
    const String getName(int fh) const;
    void *getCustom(int fh);
    bool closefile(int fh);
    uint32_t pathParts(String path) const;
    String pathPart(String path, uint32_t index) const;

    static int m_filehandle;
    /* FIXME whe have global list of open files */
    static List m_files;
};


#endif
