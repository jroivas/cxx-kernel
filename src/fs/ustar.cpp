#include "fs/ustar.hh"

#ifdef LINUX_BUILD
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <platform.h>
#endif

static const uint32_t MAX_SECTOR_SIZE = 512;
static const uint32_t MAX_BLOCK_SIZE = 512;

#ifdef USE_CUSTOM_STRING
#define returnError(X)\
do {\
    Platform::video()->printf("ERROR @%d  %s\n", __LINE__, __PRETTY_FUNCTION__);\
    return (X);\
} while(0);
#else
#define returnError(X)\
do {\
    printf("ERROR @%d  %s\n", __LINE__, __PRETTY_FUNCTION__);\
    return (X);\
} while(0);
#endif

typedef struct __attribute__((packed)) UstarData {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char type;
    char name_linked[100];
    char ustar[6];
    char version[2];
    char owner[32];
    char group[32];
    char major[8];
    char minor[8];
    char prefix[155];
} UstarData;

#define UDATA(x) ((UstarData*)x)

static uint64_t parseOct(char *data, uint32_t len)
{
    uint64_t res = 0;

    for (uint32_t i = 0; i < len; i++) {
        char c = data[i];

        if (c == 0)
            break;
        if (c < '0' || c > '9') {
            Platform::video()->printf("Invalid char: %d at %d\n", (int)c, i);
            break;
        }

        res *= 8;
        res += c - '0';
    }

    return res;
}

Ustar::Ustar()
    : m_phys(nullptr),
    m_blocksize(512)
{
#ifdef LINUX_BUILD
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    srand(tv.tv_sec + tv.tv_usec);
#endif
}

Ustar::~Ustar()
{
}

bool Ustar::verifySectorSize() const
{
    if (m_phys == nullptr) return false;
    return m_phys->sectorSize() <= MAX_SECTOR_SIZE;
}

bool Ustar::detect()
{
    if (m_phys == nullptr || !verifySectorSize()) {
        returnError(false);
    }

    uint8_t buf[MAX_SECTOR_SIZE];
    if (!m_phys->read(buf, 1, 0, 0)) {
        returnError(false);
    }

    return true;
}

bool Ustar::getBlock(uint32_t index, uint8_t *data)
{
    for (uint32_t block = 0; block < m_block_in_sectors; --block) {
        uint64_t pos = index * m_blocksize;
        pos += block * m_phys->sectorSize();

        if (!m_phys->read(
                data + m_phys->sectorSize() * block,
                1,
                pos & 0xFFFFFFFF,
                (pos >> 32) & 0xFFFFFFFF)) {
            returnError(false);
        }
    }

    return true;
}

void Ustar::clearBuffer(uint8_t *buf, uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i) {
        buf[i] = 0;
    }
}

void Ustar::setPhysical(FilesystemPhys *phys)
{
    m_phys = phys;
    m_blocks = m_phys->size() / m_blocksize;
    m_block_in_sectors = m_blocksize / m_phys->sectorSize();
}

Ustar::Iterator Ustar::list(
    uint32_t parent)
{
    Iterator iter(parent, 0);

    iter.m_data = (uint8_t*)new uint8_t[m_blocksize];
    iter.m_content = (uint8_t*)new uint8_t[m_blocksize];
    iter.m_fs = this;
    if (!getBlock(parent, iter.m_data)) {
        returnError(iter);
    }
    if (!iter.getCurrent()) {
        returnError(iter);
    }

    iter.m_ok = true;
    return iter;
}

bool Ustar::Iterator::getCurrent()
{
    uint32_t next_block;
    // TODO remove
    if (m_index * m_fs->blockSize() >= m_fs->size()) {
        return false;
    }
    if (m_block == m_index) {
        goto out;
    }

    next_block = m_block + (size() + m_fs->blockSize() - 1) / m_fs->blockSize() + 1;
    if (next_block < m_index) {
        return false;
    }
    m_index = next_block;

    if (m_index * m_fs->blockSize() >= m_fs->size()) {
        return false;
    }
    if (!m_fs->getBlock(m_index, m_data)) {
        return false;
    }
    m_block = m_index;

out:
    m_data_block = m_block + 1;
    /* Check it's ustar entry */
    if (Mem::cmp(m_data + 257, "ustar", 5))
        return false;

    m_size = parseOct(UDATA(m_data)->size, 12);

    return true;
}

ssize_t Ustar::Iterator::read(
    uint8_t *buf,
    uint64_t cnt)
{
    ssize_t readcnt = 0;
    uint32_t end_block = m_block + (size() + m_fs->blockSize() - 1) / m_fs->blockSize() + 1;

    while (cnt > 0) {
        uint32_t cblock = m_data_index / m_fs->blockSize();
        if (m_data_block + cblock >= end_block)
            return 0;
        if (!m_fs->getBlock(m_data_block + cblock, m_content))
            return 0;
        uint64_t bcnt = cnt;
        if (bcnt > m_fs->blockSize())
            bcnt = m_fs->blockSize();
        /* FIXME access in the middle / random access */
        Mem::copy(buf + readcnt, m_content, bcnt);
        cnt -= bcnt;
        m_data_index += bcnt;
        readcnt += bcnt;
    }

    return readcnt;
}

bool Ustar::Iterator::next()
{
    ++m_index;
    m_ok = getCurrent();
    m_pos = 0;
    m_data_index = 0;
    return m_ok;
}

uint32_t Ustar::Iterator::nameLen()
{
    if (m_data == nullptr) return 0;

    return String::length(UDATA(m_data)->name);
}

STD_STRING_TYPE Ustar::Iterator::name()
{
    if (m_data == nullptr) return "";

    return UDATA(m_data)->name;
}

uint64_t Ustar::Iterator::size()
{
    if (m_data == nullptr) return 0;

    return m_size;
}

uint8_t Ustar::Iterator::type() const
{
    if (m_data == nullptr) return 0;

    switch (UDATA(m_data)->type) {
    case '0': return TYPE_FILE;
    case '1': return TYPE_HARD_LINK;
    case '2': return TYPE_SYM_LINK;
    case '3': return TYPE_CHAR_DEV;
    case '4': return TYPE_BLOCK_DEV;
    case '5': return TYPE_DIR;
    case '6': return TYPE_FIFO;
    default:
        return TYPE_INVALID;
    }
}

UstarFilesystem::UstarFilesystem()
    : m_ustar(new Ustar)
{
}

UstarFilesystem::UstarFilesystem(const UstarFilesystem &fs)
{
    m_ustar = fs.m_ustar;
    m_mountpoint = fs.m_mountpoint;
    m_opts = fs.m_mountpoint;
}

Filesystem *UstarFilesystem::mount(String mountpoint, String options)
{
    UstarFilesystem *tmp = new UstarFilesystem();
    tmp->m_ustar = m_ustar;
    tmp->m_mountpoint = mountpoint;
    tmp->m_opts = options;
    return tmp;
}

bool UstarFilesystem::umount(Filesystem *fs)
{
    UstarFilesystem *tmp = (UstarFilesystem*)fs;
    delete tmp;

    return true;
}

int UstarFilesystem::truncate(String path, off_t length)
{
    (void)path;
    (void)length;
    errno = EPERM;
    return -1;
}

Ustar::Iterator UstarFilesystem::findFile(
    String path,
    uint32_t part,
    uint32_t parent)
{
    (void)part;
    Ustar::Iterator iter;

    iter = m_ustar->list(parent);

    /* FIXME This is very simple find wiaht full path*/
    while (iter.ok()) {
        if (iter.name() == path) {
            return iter;
        }
        if (!iter.next()) break;
    }

    return iter;
}

int UstarFilesystem::open(String path, int flags)
{
    (void)flags;

    Ustar::Iterator iter = findFile(path);
    if (!iter.ok()) {
        errno = ENOENT;
        return -1;
    }
    if ((iter.type() != Ustar::TYPE_FILE)) {
        errno = EPERM;
        return -1;
    }

    Ustar::Iterator *data = new Ustar::Iterator();
    data->assign(iter);

    int res = mapfile(type(), path, this, data);
    return res;
}

int UstarFilesystem::close(int fh)
{
    Ustar::Iterator *data = (Ustar::Iterator *)getCustom(fh);
    if (data != nullptr) {
        delete data;
    }
    if (!closefile(fh)) {
        errno = EBADF;
        return -1;
    }
    return 0;
}

int UstarFilesystem::chmod(String path, mode_t mode)
{
    (void)path;
    (void)mode;
    errno = EPERM;
    return -1;
}

int UstarFilesystem::getattr(String path, int handle)
{
    (void)path;
    (void)handle;
    errno = EPERM;
    return -1;
}

ssize_t UstarFilesystem::read(int fh, char *buf, size_t count)
{
    (void)buf;
    (void)count;
    String name = getName(fh);

    Ustar::Iterator *data = (Ustar::Iterator *)getCustom(fh);
    if (data == nullptr) {
        errno = ENOENT;
        return -1;
    }

    return data->read((uint8_t*)buf, count);
}

int UstarFilesystem::fseek(
    int fd, long offs_hi, long offs_low,
    loff_t *result, unsigned int orig)
{
    (void)fd;
    (void)offs_hi;
    (void)offs_low;
    (void)result;
    (void)orig;

    errno = EBADF;
    return -1;
}

ssize_t UstarFilesystem::write(int fh, const char *buf, size_t count)
{
    (void)fh;
    (void)buf;
    (void)count;
    errno = EPERM;
    return -1;
}

int UstarFilesystem::symlink(String target, String linkpath)
{
    (void)target;
    (void)linkpath;
    errno = EPERM;
    return -1;
}

int UstarFilesystem::rename(String oldpath, String newpath)
{
    (void)oldpath;
    (void)newpath;
    errno = EPERM;
    return -1;
}

int UstarFilesystem::unlink(String path)
{
    (void)path;
    errno = EPERM;
    return -1;
}

int UstarFilesystem::stat(int fd, struct stat *st)
{
    (void)fd;
    (void)st;
    return -1;
}

int UstarFilesystem::flush(int fd)
{
    (void)fd;
    return 0;
}

Dir *UstarFilesystem::opendir(String name)
{
    (void)name;
    return nullptr;
}

Dirent *UstarFilesystem::readdir(Dir *dir)
{
    (void)dir;
    return nullptr;
}

void UstarFilesystem::rewinddir(Dir *dir)
{
    (void)dir;
}

int UstarFilesystem::closedir(Dirent *dir)
{
    (void)dir;
    return -1;
}

int UstarFilesystem::mkdir(String path, mode_t mode)
{
    (void)path;
    (void)mode;
    errno = EPERM;
    return -1;
}

int UstarFilesystem::rmdir(String path)
{
    (void)path;
    errno = EPERM;
    return -1;
}
