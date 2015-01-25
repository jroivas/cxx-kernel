#ifndef __VFS_HH
#define __VFS_HH

#include <list.hh>
#include <fs/filesystem.hh>

class VFS
{
public:
    VFS();
    bool register_filesystem(Filesystem *type);

protected:
    List m_filesystems;
};

#endif
