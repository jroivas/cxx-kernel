#include <fs/filesystem.hh>
#include <mutex.hh>
#include <mm.h>
#include <platform.h>
#include <errno.h>

int Filesystem::m_filehandle = 2;
static Mutex __fs_mutex;
List Filesystem::m_files;

class FileData
{
public:
    FileData(int id, String fs, String name, Filesystem *owner, void *custom)
        : m_fs(fs),
        m_name(name),
        m_id(id),
        m_owner(owner),
        m_custom(custom),
        m_fcntl(0)
    {
    }

    String m_fs;
    String m_name;
    int m_id;
    Filesystem *m_owner;
    void *m_custom;
    int m_fcntl;
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
        return nullptr;
    }

    Dirent *res = (Dirent *)m_items.at(m_index);
    ++m_index;

    return res;
}

int Filesystem::mapfile(
    const String &fs,
    const String &name,
    Filesystem *owner,
    void *custom)
{
    MutexLocker mtx(&__fs_mutex);

    ++m_filehandle;
    FileData *tmp = new FileData(
        m_filehandle,
        fs,
        name,
        owner,
        custom);

    m_files.append(tmp);

    return tmp->m_id;
}

int Filesystem::getfile(const String &fs, const String &name) const
{
    MutexLocker mtx(&__fs_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = reinterpret_cast<FileData*>(m_files.at(i));
        if (tmp->m_fs == fs && tmp->m_name == name) {
            return tmp->m_id;
        }
    }

    return -1;
}

const String Filesystem::getFS(int fh) const
{
    MutexLocker mtx(&__fs_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = reinterpret_cast<FileData*>(m_files.at(i));
        if (tmp->m_id == fh) {
            return tmp->m_fs;
        }
    }

    return "";
}

void *Filesystem::getCustom(int fh)
{
    MutexLocker mtx(&__fs_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = reinterpret_cast<FileData*>(m_files.at(i));
        if (tmp->m_id == fh) {
            return tmp->m_custom;
        }
    }

    return nullptr;
}

Filesystem *Filesystem::getFilesystem(int fh)
{
    MutexLocker mtx(&__fs_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = reinterpret_cast<FileData*>(m_files.at(i));
        if (tmp->m_id == fh) {
            return tmp->m_owner;
        }
    }

    return nullptr;
}

const String Filesystem::getName(int fh) const
{
    MutexLocker mtx(&__fs_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = reinterpret_cast<FileData*>(m_files.at(i));
        if (tmp->m_id == fh) {
            return tmp->m_name;
        }
    }

    return "";
}

bool Filesystem::closefile(int fh)
{
    MutexLocker mtx(&__fs_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = reinterpret_cast<FileData*>(m_files.at(i));
        if (tmp->m_id == fh) {
            m_files.deleteAt(i);
            return true;
        }
    }

    return false;
}

int Filesystem::getFcntl(int fh)
{
    MutexLocker mtx(&__fs_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = reinterpret_cast<FileData*>(m_files.at(i));
        if (tmp->m_id == fh) {
            return tmp->m_fcntl;
        }
    }

    errno = EBADF;
    return -1;
}

int Filesystem::setFcntl(int fh, int mode)
{
    MutexLocker mtx(&__fs_mutex);

    for (uint32_t i = 0; i < m_files.size(); ++i) {
        FileData *tmp = reinterpret_cast<FileData*>(m_files.at(i));
        if (tmp->m_id == fh) {
            tmp->m_fcntl = mode;
            return 0;
        }
    }

    errno = EBADF;
    return -1;
}

uint32_t Filesystem::pathParts(String path) const
{
    uint32_t cnt = 0;
    size_t len = path.length();
    for (size_t p = 0; p < len; ++p) {
        char c = path[p];
        if (c == '/') {
            ++cnt;
        }
    }
    return cnt;
}

String Filesystem::pathPart(String path, uint32_t index) const
{
    uint32_t cnt = 0;
    String res;

    size_t len = path.length();
    for (size_t p = 0; p < len; ++p) {
        char c = path[p];
        if (c == '/') {
            ++cnt;
            if (res.length() > 0) break;
        } else if (cnt == index) {
            res += c;
        }
    }

    return res;
}
