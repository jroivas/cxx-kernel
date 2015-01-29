#include <fs/filesystem.hh>
#include <mutex.h>
#include <mm.h>

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
    Mutex mtx(&m_mutex);
    mtx.lock();

    ++m_filehandle;
    FileData *tmp = new FileData(
        m_filehandle,
        fs,
        name,
        owner);

    m_files.append(tmp);

    mtx.unlock();

    return tmp->m_id;
}

int Filesystem::getfile(const String &fs, const String &name)
{
    Mutex mtx(&m_mutex);
    mtx.lock();

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_fs == fs && tmp->m_name == name) {
            mtx.unlock();
            return tmp->m_id;
        }
    }
    mtx.unlock();

    return -1;
}

String Filesystem::getFS(int fh)
{
    Mutex mtx(&m_mutex);
    mtx.lock();

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_id == fh) {
            mtx.unlock();
            return tmp->m_fs;
        }
    }
    mtx.unlock();

    return "";
}

Filesystem *Filesystem::getFilesystem(int fh)
{
    Mutex mtx(&m_mutex);
    mtx.lock();

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_id == fh) {
            mtx.unlock();
            return tmp->m_owner;
        }
    }
    mtx.unlock();

    return NULL;
}

String Filesystem::getName(int fh)
{
    Mutex mtx(&m_mutex);
    mtx.lock();

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_id == fh) {
            mtx.unlock();
            return tmp->m_name;
        }
    }
    mtx.unlock();

    return "";
}

bool Filesystem::closefile(int fh)
{
    Mutex mtx(&m_mutex);
    mtx.lock();

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = (FileData*)m_files.at(i);
        if (tmp->m_id == fh) {
            m_files.deleteAt(i);
            mtx.unlock();
            return true;
        }
    }
    mtx.unlock();

    return false;
}
