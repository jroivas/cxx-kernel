#include <fs/filesystem.hh>
#include <string.hh>
#include <platform.h>

class DevFS : public Filesystem
{
public:
    DevFS() {}
    DevFS(const DevFS &fs);
    virtual ~DevFS() {}

    virtual inline const String type() const
    {
        return String("DevFS");
    }
    virtual void setPhysical(FilesystemPhys *)
    {
    }
    virtual Filesystem *mount(String mountpoint, String options);
    virtual const String &mountpoint() const
    {
        return m_mountpoint;
    }
    virtual bool umount(Filesystem *fs);

    // File operations
    virtual int truncate(String path, off_t length);

    virtual int open(String path, int flags);
    virtual int close(int fh);

    virtual int chmod(String path, mode_t mode);
    virtual int getattr(String path, int handle=0);

    virtual ssize_t read(int fh, char *buf, size_t count);
    virtual ssize_t write(int fh, const char *buf, size_t count);

    virtual int fseek(
        int fd, long offs_hi, long offs_low,
        loff_t *result, unsigned int orig);

    virtual int symlink(String target, String linkpath);
    virtual int rename(String oldpath, String newpath);
    virtual int unlink(String path);

    virtual int stat(int fd, struct stat *st);
    virtual int flush(int fd);

    // Dir operations
    virtual Dir *opendir(String name);
    virtual Dirent *readdir(Dir *dir);
    virtual void rewinddir(Dir *dir);
    virtual int closedir(Dirent *dir);

    virtual int mkdir(String path, mode_t mode);
    virtual int rmdir(String path);

protected:
    String m_mountpoint;
    String m_opts;
};
