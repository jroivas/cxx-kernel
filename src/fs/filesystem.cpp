#include <fs/filesystem.hh>
#include <mutex.hh>
#include <mm.h>
#include <platform.h>

int Filesystem::m_filehandle = 2;
ptr_val_t Filesystem::m_mutex = 0;
List Filesystem::m_files;

class FileData
{
public:
    FileData(int id, String fs, String name, Filesystem *owner)
        : m_fs(fs),
        m_name(name),
        m_id(id),
        m_owner(owner)
    {
    }

    String m_fs;
    String m_name;
    int m_id;
    Filesystem *m_owner;
};

void Dir::reset()
{
    m_index = 0;
}

void Dir::update()
{
}

Dirent *Dir::next()
{
    if (m_index >= m_items.size()) {
        return NULL;
    }

    Dirent *res = (Dirent *)m_items.at(m_index);
    ++m_index;

    return res;
}

int Filesystem::mapfile(const String &fs, const String &name, Filesystem *owner)
{
    LockMutex mtx(&m_mutex);

    ++m_filehandle;
    FileData *tmp = new FileData(
        m_filehandle,
        fs,
        name,
        owner);

    m_files.append(tmp);

    return tmp->m_id;
}

int Filesystem::getfile(const String &fs, const String &name) const
{
    LockMutex mtx(&m_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_fs == fs && tmp->m_name == name) {
            return tmp->m_id;
        }
    }

    return -1;
}

const String Filesystem::getFS(int fh) const
{
    LockMutex mtx(&m_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_id == fh) {
            return tmp->m_fs;
        }
    }

    return "";
}

Filesystem *Filesystem::getFilesystem(int fh)
{
    LockMutex mtx(&m_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_id == fh) {
            return tmp->m_owner;
        }
    }

    return NULL;
}

const String Filesystem::getName(int fh) const
{
    LockMutex mtx(&m_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_id == fh) {
            return tmp->m_name;
        }
    }

    return "";
}

bool Filesystem::closefile(int fh)
{
    LockMutex mtx(&m_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_id == fh) {
            m_files.deleteAt(i);
            return true;
        }
    }

    return false;
}
