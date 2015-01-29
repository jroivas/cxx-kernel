#ifndef __VFS_HH
#define __VFS_HH

#include <list.hh>
#include <fs/filesystem.hh>

class VFS
{
public:
    VFS() {}
    ~VFS() {}
    bool register_filesystem(Filesystem *type);
    bool mount(String mountpoint, String type, String options);

    Filesystem *access(String file);
    String basedir(String file, Filesystem *fs);
    String stripslash(String name);

protected:
    Filesystem *find_filesystem(String type);

    List m_filesystems;
    List m_mounts;
};

#endif
