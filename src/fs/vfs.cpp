#include "fs/vfs.hh"
#include "platform.h"

bool VFS::register_filesystem(Filesystem *type)
{
    m_filesystems.append(type);
    return true;
}

Filesystem *VFS::find_filesystem(String type)
{
    const List::ListObject *iter = m_filesystems.begin();
    if (iter == nullptr) {
        return nullptr;
    }

    Filesystem *res = (Filesystem*)m_filesystems.item(iter);
    while (res != nullptr) {
        if (res->type() == type) {
            return res;
        }

        iter = m_filesystems.next(iter);
        if (iter == nullptr) {
            break;
        }
        res = (Filesystem*)m_filesystems.item(iter);
    }

    return nullptr;
}

bool VFS::mount(String mountpoint, String type, String options)
{
    Filesystem *fs = find_filesystem(type);
    if (fs == nullptr) {
        return false;
    }
    Filesystem *mount = fs->mount(mountpoint, options);
    if (mount == nullptr) {
        return false;
    }

    m_mounts.append(mount);

    return true;
}

Filesystem *VFS::accessHandle(int fd)
{
    const List::ListObject *iter = m_mounts.begin();
    while (iter != nullptr) {
        Filesystem *tmp = (Filesystem*)m_mounts.item(iter);
        if (tmp == nullptr) {
            iter = m_mounts.next(iter);
            continue;
        }
        return tmp->getFilesystem(fd);
    }
    return nullptr;
}

Filesystem *VFS::access(String file)
{
    const List::ListObject *iter = m_mounts.begin();
    while (iter != nullptr) {
        Filesystem *tmp = (Filesystem*)m_mounts.item(iter);
        if (tmp != nullptr) {
            if (file.startsWith(tmp->mountpoint())) {
                return tmp;
            }
        }
        iter = m_mounts.next(iter);
    }

    return nullptr;
}

String VFS::basedir(String file, Filesystem *fs)
{
    if (fs == nullptr) {
        return file;
    }
    return file.right(fs->mountpoint().length() + 1);
}

String VFS::stripslash(String name)
{
    String res = name;
    while (res[0] == '/') {
        res = res.right(1);
    }
    return res;
}
