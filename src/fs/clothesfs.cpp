#include "clothesfs.hh"
//#include <stdlib.h>
//#include <stdio.h>
//#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <platform.h>

static uint32_t header_begin = 8 * 4;
static uint32_t metadata_id = 0x42;
static uint32_t payload_id = 0x4242;

#ifdef USE_CUSTOM_STRING
//#define returnError(X) return (X);
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

ClothesFS::ClothesFS()
    : m_phys(NULL),
    m_blocksize(512)
{
#ifndef USE_CUSTOM_STRING
    struct timeval tv;
    gettimeofday(&tv, NULL);

    srand(tv.tv_sec + tv.tv_usec);
#endif
}

ClothesFS::~ClothesFS()
{
}

uint32_t ClothesFS::dataToNum(uint8_t *buf, int start, int cnt)
{
    uint32_t res = 0;
    int n = 0;
    for (int i = start; i < start + cnt; ++i) {
        res += buf[i] << n;
        n += 8;
    }
    return res;
}

void ClothesFS::numToData(uint64_t num, uint8_t *buf, int start, int cnt)
{
    int n = 0;
    for (int i = start; i < start + cnt; ++i) {
        buf[i] = (num >> n) & 0xFF;
        n += 8;
    }
}

bool ClothesFS::detect()
{
    if (m_phys == NULL) {
        returnError(false);
    }

    uint8_t buf[m_phys->sectorSize()];
    if (!m_phys->read(buf, 1, 0, 0)) {
        returnError(false);
    }

    m_blocksize = dataToNum(buf, header_begin + 4, 2);

    return (
           buf[header_begin + 0] == 0x00
        && buf[header_begin + 1] == 0x42
        && buf[header_begin + 2] == 0x00
        && buf[header_begin + 3] == 0x41);
}

bool ClothesFS::getBlock(uint32_t index, uint8_t *data)
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

bool ClothesFS::putBlock(uint32_t index, uint8_t *data)
{
    for (uint32_t block = 0; block < m_block_in_sectors; --block) {
        uint64_t pos = index * m_blocksize;
        pos += block * m_phys->sectorSize();

        if (!m_phys->write(
                data + m_phys->sectorSize() * block,
                1,
                pos & 0xFFFFFFFF,
                (pos >> 32) & 0xFFFFFFFF)) {
            returnError(false);
        }
    }

    return true;
}


bool ClothesFS::formatBlock(uint32_t num, uint32_t next)
{
    uint8_t buf[m_blocksize];
    clearBuffer(buf, m_blocksize);

    numToData(metadata_id, buf, 0, 4);
    numToData(next, buf, m_blocksize - 4, 4);

    return putBlock(num, buf);
}

uint32_t ClothesFS::formatBlocks()
{
    uint32_t next = 0;
    uint32_t start = 2;
    for (uint32_t i = m_blocks - 1; i >= start; --i) {
        if (!formatBlock(i, next)) {
            return 0;
        }
        next = i;
    }
    return start;
}

void ClothesFS::clearBuffer(uint8_t *buf, uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i) {
        buf[i] = 0;
    }
}

void ClothesFS::setPhysical(FilesystemPhys *phys)
{
    m_phys = phys;
    m_blocks = m_phys->size() / m_blocksize;
    m_block_in_sectors = m_blocksize / m_phys->sectorSize();
}

bool ClothesFS::format(
    const char *volid)
{
    uint8_t buf[m_phys->sectorSize()];
    clearBuffer(buf, m_phys->sectorSize());

    buf[header_begin + 0] = 0x00;
    buf[header_begin + 1] = 0x42;
    buf[header_begin + 2] = 0x00;
    buf[header_begin + 3] = 0x41;

    uint32_t pos = header_begin + 4;
    numToData(m_blocksize, buf, pos, 2);

    pos += 2;

    //flags
    buf[pos] = 0x0;
    //grpindex
    buf[pos + 1] = 0x0;

    pos += 2;

    // vol id
#ifndef USE_CUSTOM_STRING
    for (int i = 0; i < 8; ++i) {
        buf[pos + i] = rand() % 0xFF;
    }
#endif
    pos += 8;

    // size
    numToData(m_phys->size(), buf, pos, 4);
    pos += 8;
    m_blocks = m_phys->size() / m_blocksize;
    m_block_in_sectors = m_blocksize / m_phys->sectorSize();

    // vol name
    if (volid != NULL) {
        for (uint32_t i = 0; i < 32; ++i) {
            if (*volid == 0) break;
            buf[pos + i] = *volid;
            ++volid;
        }
    }
    pos += 32;

    // Block 1 is root dir (FIXME)
    numToData(1, buf, pos, 4);
    pos += 4;

    uint32_t freechain = formatBlocks();

    // Used
    numToData(2, buf, pos, 4);
    pos += 4;

    // Journal1
    numToData(0, buf, pos, 4);
    pos += 4;

    // Journal2
    numToData(0, buf, pos, 4);
    pos += 4;

    // Freechain
    numToData(freechain, buf, pos, 4);
    pos += 4;

    bool res = m_phys->write(buf, 1, 0, 0);

    if (res and m_blocksize > 512) {
        // TODO
    }

    if (!initMeta(1, META_DIR)) {
        returnError(false);
    }
    if (!updateMeta(1, (const uint8_t*)"/", 0x0)) {
        returnError(false);
    }

    return res;
}

bool ClothesFS::initMeta(
    uint32_t index,
    uint8_t type)
{
    uint8_t data[m_blocksize];
    clearBuffer(data, m_blocksize);

    numToData(metadata_id, data, 0, 2);
    numToData(type, data, 2, 1);
    numToData(ATTRIB_NONE, data, 3, 1);

    return putBlock(index, data);
}

uint32_t ClothesFS::initData(
    uint32_t index,
    uint8_t type,
    uint8_t algo)
{
    uint8_t data[m_blocksize];
    clearBuffer(data, m_blocksize);

    numToData(payload_id, data, 0, 2);
    numToData(type, data, 2, 1);
    numToData(algo, data, 3, 1);

    if (!putBlock(index, data)) {
        return 0;
    }

    return 4;
}

uint32_t ClothesFS::takeFreeBlock()
{
    uint8_t data[m_blocksize];
    uint8_t block[m_blocksize];
    if (!getBlock(0, data)) {
        return 0;
    }

    uint32_t freechain = dataToNum(data, 104, 4);

    if (!getBlock(freechain, block)) {
        return 0;
    }
    uint32_t next_freechain = dataToNum(block, m_blocksize - 4, 4);

    numToData(next_freechain, data, 104, 4);
    if (!putBlock(0, data)) {
        return 0;
    }

    return freechain;
}

bool ClothesFS::addFreeBlock(uint32_t id)
{
    if (id == 0) return false;

    uint8_t data[m_blocksize];
    uint8_t block[m_blocksize];

    if (!getBlock(0, data)) {
        return false;
    }
    if (!getBlock(id, block)) {
        return false;
    }
    uint8_t status = dataToNum(block, 2, 1);
    if (status == META_FREE) {
        return false;
    }

    uint32_t freechain = dataToNum(data, 104, 4);

    numToData(freechain, block, m_blocksize - 4, 4);
    numToData(0x42, block, 0, 4);
    numToData(id, data, 104, 4);

    if (!putBlock(0, data)) {
        return false;
    }

    return true;
}

bool ClothesFS::dirContinues(
    uint32_t index,
    uint32_t next)
{
    uint8_t data[m_blocksize];
    if (!getBlock(index, data)) {
        returnError(false);
    }

    numToData(next, data, m_blocksize - 4, 4);
    return putBlock(index, data);
}

uint8_t ClothesFS::baseType(uint8_t type) const
{
    if (type == META_FILE_CONT) type = META_FILE;
    else if (type == META_DIR_CONT) type = META_DIR;
    return type;
}

bool ClothesFS::validType(uint8_t type, uint8_t valid) const
{
    type = baseType(type);
    valid = baseType(valid);

    return type == valid;
}

bool ClothesFS::addToMeta(
    uint32_t index,
    uint32_t meta,
    uint8_t type)
{
    uint8_t data[m_blocksize];
    if (!getBlock(index, data)) {
        returnError(false);
    }

    uint32_t id = dataToNum(data, 0, 2);
    if (id != metadata_id) {
        returnError(false);
    }

    uint32_t data_type = dataToNum(data, 2, 1);
    //FIXME hardcode
    if (!validType(data_type, type)) {
        returnError(false);
    }

    uint32_t ptr = 4;
    if (type == META_FILE
        || type == META_DIR) {
        ptr += 8;
        uint32_t namelen = dataToNum(data, ptr, 4);
        ptr += namelen;
        while (ptr % 4 != 0) {
            ++ptr;
        }
    }
    while (ptr < m_blocksize - 4) {
        uint32_t val = dataToNum(data, ptr, 4);
        if (val == 0) {
            numToData(meta, data, ptr, 4);
            return putBlock(index, data);
        }
        ptr += 4;
    }
    uint32_t next = dataToNum(data, m_blocksize - 4, 4);
    if (next != 0) {
        return addToMeta(next, meta, type);
    }

    next = takeFreeBlock();
    uint32_t next_type = META_DIR_CONT;
    if (type == META_FILE
        || type == META_FILE_CONT) {
        next_type = META_FILE_CONT;
    }
    if (next != 0) {
        if (dirContinues(index, next)
            && initMeta(next, next_type)) {
            return addToMeta(next, meta, type);
        }
    }

    returnError(false);
}

bool ClothesFS::addData(
    uint32_t meta,
    const char *contents,
    uint64_t size)
{
    uint32_t data_block = takeFreeBlock();
    if (data_block == 0) {
        returnError(false);
    }
    if (!addToMeta(meta, data_block, META_FILE)) {
        returnError(false);
    }

    uint32_t pos = initData(data_block, PAYLOAD_USED, 0);
    uint8_t data[m_blocksize];
    if (!getBlock(data_block, data)) {
        returnError(false);
    }

    const char *input = contents;
    uint64_t data_size = size;
    while (data_size > 0 && pos < m_blocksize) {
        data[pos] = *input;
        ++pos;
        ++input;
        --data_size;
    }

    if (!putBlock(data_block, data)) {
        returnError(false);
    }
    if (data_size > 0) {
        return addData(meta, input, data_size);
    }

    return true;
}

bool ClothesFS::updateMeta(
    uint32_t index,
    const uint8_t *name,
    uint64_t size)
{
    uint8_t data[m_blocksize];
    if (!getBlock(index, data)) {
        returnError(false);
    }

    uint32_t type = baseType(dataToNum(data, 2, 1));
    if (type != META_FILE
        && type != META_DIR) {
        returnError(false);
    }

    numToData(size, data, 4, 8);
    uint32_t len = 0;
    uint32_t pos = 16;
    while (name != NULL
        && *name != 0) {
        data[pos] = *name;
        ++pos;
        ++name;
        ++len;
    }
    numToData(len, data, 12, 4);

    while (pos % 4 != 0) {
        ++pos;
    }

    return putBlock(index, data);
}

bool ClothesFS::addFile(
    uint32_t parent,
    const char *name,
    const char *contents,
    uint64_t size)
{
    if (parent == 0) {
        returnError(false);
    }
    uint32_t block = takeFreeBlock();
    if (block == 0) {
        returnError(false);
    }
    if (!addToMeta(parent, block, META_DIR)) {
        returnError(false);
    }
    if (!initMeta(block, META_FILE)) {
        returnError(false);
    }
    if (!updateMeta(block, (const uint8_t*)name, size)) {
        returnError(false);
    }

    return addData(block, contents, size);
}

bool ClothesFS::addDir(
    uint32_t parent,
    const char *name)
{
    if (parent == 0) {
        returnError(false);
    }
    uint32_t block = takeFreeBlock();
    if (block == 0) {
        returnError(false);
    }
    if (!addToMeta(parent, block, META_DIR)) {
        returnError(false);
    }
    if (!initMeta(block, META_DIR)) {
        returnError(false);
    }
    if (!updateMeta(block, (const uint8_t*)name, 0)) {
        returnError(false);
    }
    return true;
}

ClothesFS::Iterator ClothesFS::list(
    uint32_t parent)
{
    Iterator iter(parent, 0);
    if (parent == 0) {
        returnError(iter);
    }

    // FIXME free old
    iter.m_parent = (uint8_t*)new uint8_t[m_blocksize];
    iter.m_data = (uint8_t*)new uint8_t[m_blocksize];
    iter.m_content = (uint8_t*)new uint8_t[m_blocksize];
    iter.m_fs = this;

    if (!getBlock(parent, iter.m_parent)) {
        returnError(iter);
    }
    if (!iter.getCurrent()) {
        returnError(iter);
    }

    iter.m_ok = true;
    return iter;
}

bool ClothesFS::Iterator::getCurrent()
{
    uint32_t type = dataToNum(m_parent, 2, 1);
    if (type == 0) {
        returnError(false);
    }

    uint32_t start = 4;
    if (type == META_FILE
        || type == META_DIR) {
        start += 8;
        uint32_t namelen = m_fs->dataToNum(m_parent, start, 4);
        start += 4;
        start += namelen;
        while (start % 4 != 0) {
            ++start;
        }
    }
    uint32_t pos = 4  * m_index + start;
    if (pos >= m_fs->blockSize() - 4) {
        uint32_t next_block = m_fs->dataToNum(
            m_parent,
            m_fs->blockSize() - 4,
            4);
        if (next_block == 0) {
            return false;
        }
        if (!m_fs->getBlock(next_block, m_parent)) {
            return false;
        }
        m_index = 0;
        pos = 4  * m_index;
    }

    m_block = m_fs->dataToNum(m_parent, pos, 4);
    if (m_block == 0) {
        return false;
    }
    return m_fs->getBlock(m_block, m_data);
}

uint64_t ClothesFS::Iterator::read(
    uint8_t *buf,
    uint64_t cnt)
{
    uint32_t type = dataToNum(m_data, 2, 1);
/*
    if (type != META_FILE
        || type != META_FILE_CONT) {
        returnError(0);
    }
*/

    if ((m_pos == 0 && m_data_block == 0)
        || m_pos >= m_fs->blockSize()) {
        // FIXME: store pos
        uint32_t start = 4;
        if (type == META_FILE) {
            start += 8;
            uint32_t namelen = m_fs->dataToNum(m_data, start, 4);
            start += 4;
            start += namelen;
            while (start % 4 != 0) {
                ++start;
            }
        }

        uint32_t pos = 4  * m_data_index + start;
        if (pos >= m_fs->blockSize() - 4) {
            uint32_t next_block = m_fs->dataToNum(
                m_data,
                m_fs->blockSize() - 4,
                4);
            if (next_block == 0) {
                return false;
            }
            if (!m_fs->getBlock(next_block, m_data)) {
                return false;
            }
            m_data_index = 0;
            pos = 4  * m_data_index + 4;
        }

        m_data_block = m_fs->dataToNum(m_data, pos, 4);
        if (m_data_block == 0) {
            returnError(0);
        }
        if (!m_fs->getBlock(m_data_block, m_content)) {
            return 0;
        }
        if (m_fs->dataToNum(m_content, 0, 2) != 0x4242) {
            return 0;
        }
        if (m_fs->dataToNum(m_content, 2, 1) != PAYLOAD_USED) {
            return 0;
        }
        //FIXME algo
        m_pos = 4;
    }

    uint8_t *dest = buf;
    uint64_t got = 0;
    while (m_pos < m_fs->blockSize() && cnt > 0) {
        *dest = m_content[m_pos];
        ++m_pos;
        ++dest;
        --cnt;
        ++got;
    }

    return got;
}

bool ClothesFS::Iterator::next()
{
    ++m_index;
    m_ok = getCurrent();
    m_pos = 0;
    m_data_block = 0;
    m_data_index = 0;
    return m_ok;
}

uint32_t ClothesFS::Iterator::nameLen()
{
    if (m_data == NULL) return 0;

    return dataToNum(m_data, 12, 4);
}

STD_STRING_TYPE ClothesFS::Iterator::name()
{
    if (m_data == NULL) return "";

    STD_STRING_TYPE res;
    uint32_t namelen = nameLen();
    for (uint32_t i = 0; i < namelen; ++i) {
        res += (char)m_data[16 + i];
    }
    return res;
}

uint64_t ClothesFS::Iterator::size()
{
    if (m_data == NULL) return 0;

    return dataToNum(m_data, 4, 8);
}

uint8_t ClothesFS::Iterator::type() const
{
    if (m_data == NULL) return 0;

    return m_fs->baseType(dataToNum(m_data, 2, 1));
}

bool ClothesFS::Iterator::remove()
{
    if (m_data == NULL) return false;
    m_pos = 0;
    m_data_block = 0;
    m_data_index = 0;
    uint32_t type = dataToNum(m_data, 2, 1);
    uint32_t the_block = m_block;

    uint32_t start = 4;
    if (type == META_FILE
        || type == META_DIR) {
        start += 8;
        uint32_t namelen = m_fs->dataToNum(m_data, start, 4);
        start += 4;
        start += namelen;
        while (start % 4 != 0) {
            ++start;
        }
    }
    uint32_t pos = 4  * m_data_index + start;
    while (true) {
        if (pos >= m_fs->blockSize() - 4) {
            m_fs->addFreeBlock(the_block);
            uint32_t next_block = m_fs->dataToNum(
                m_data,
                m_fs->blockSize() - 4,
                4);
            if (next_block == 0) {
                break;
            }
            if (!m_fs->getBlock(next_block, m_data)) {
                break;
            }
            the_block = next_block;
            m_data_index = 0;
            pos = 4  * m_data_index + 4;
        }

        m_data_block = m_fs->dataToNum(m_data, pos, 4);
        if (m_data_block == 0) {
            break;
        }
        m_fs->addFreeBlock(m_data_block);
        pos += 4;
    }

    m_fs->addFreeBlock(the_block);

    return true;
}


/* Kernel adaptation */

ClothesFilesystem::ClothesFilesystem()
    : m_clothes(new ClothesFS)
{
}

ClothesFilesystem::ClothesFilesystem(const ClothesFilesystem &fs)
{
    m_clothes = fs.m_clothes;
    m_mountpoint = fs.m_mountpoint;
    m_opts = fs.m_mountpoint;
}

Filesystem *ClothesFilesystem::mount(String mountpoint, String options)
{
    ClothesFilesystem *tmp = new ClothesFilesystem();
    tmp->m_clothes = m_clothes;
    tmp->m_mountpoint = mountpoint;
    tmp->m_opts = options;
    return tmp;
}

bool ClothesFilesystem::umount(Filesystem *fs)
{
    ClothesFilesystem *tmp = (ClothesFilesystem*)fs;
    delete tmp;

    return true;
}

int ClothesFilesystem::truncate(String path, off_t length)
{
    (void)path;
    (void)length;
    errno = EPERM;
    return -1;
}

ClothesFS::Iterator ClothesFilesystem::findFile(
    String path,
    uint32_t part,
    uint32_t parent)
{
    ClothesFS::Iterator iter;
    uint32_t parts = pathParts(path);

    if (part > parts) {
        return iter;
    }
    String fname = pathPart(path, part);

    iter = m_clothes->list(parent);

    while (iter.ok()) {
        if (iter.name() == fname) {
            if (part == parts - 1 || part == parts) {
                return iter;
            } else if (part < parts - 1) {
                if (iter.type() & ClothesFS::META_DIR) {
                    return findFile(path, part + 1, iter.block());
                }
                return iter;
            }
        }
        if (!iter.next()) break;
    }

    return iter;
}

class ClothesPrivate
{
public:
};

int ClothesFilesystem::open(String path, int flags)
{
    (void)flags;

    ClothesFS::Iterator iter = findFile(path);
    //TODO Create file
    if (!iter.ok()) {
        errno = ENOENT;
        return -1;
    }
    if ((iter.type() & ClothesFS::META_FILE) == 0) {
        errno = EPERM;
        return -1;
    }

    ClothesFS::Iterator *data = new ClothesFS::Iterator();
    data->assign(iter);

    int res = mapfile(type(), path, this, data);
    return res;
#if 0

    (void)path;
    if ((flags & O_APPEND)
        || (flags & O_CREAT)
        || (flags & O_TRUNC)
        || (flags & O_TMPFILE)
        || (flags & O_WRONLY)
        || (flags & O_RDWR)) {
        errno = EPERM;
        return -1;
    }
#endif
}

int ClothesFilesystem::close(int fh)
{
    ClothesFS::Iterator *data = (ClothesFS::Iterator *)getCustom(fh);
    if (data != NULL) {
        delete data;
    }
    if (!closefile(fh)) {
        errno = EBADF;
        return -1;
    }
    return 0;
}

int ClothesFilesystem::chmod(String path, mode_t mode)
{
    (void)path;
    (void)mode;
    errno = EPERM;
    return -1;
}

int ClothesFilesystem::getattr(String path, int handle)
{
    (void)path;
    (void)handle;
    errno = EPERM;
    return -1;
}

ssize_t ClothesFilesystem::read(int fh, char *buf, size_t count)
{
    (void)buf;
    (void)count;
    String name = getName(fh);

    ClothesFS::Iterator *data = (ClothesFS::Iterator *)getCustom(fh);
    if (data == NULL) {
        errno = ENOENT;
        return -1;
    }

    return data->read((uint8_t*)buf, count);
}

int ClothesFilesystem::fseek(
    int fd, long offs_hi, long offs_low,
    loff_t *result, unsigned int orig)
{
    String name = getName(fd);

    (void)offs_hi;
    (void)offs_low;
    (void)result;
    (void)orig;

    errno = EBADF;
    return -1;
}

ssize_t ClothesFilesystem::write(int fh, const char *buf, size_t count)
{
    (void)fh;
    (void)buf;
    (void)count;
    errno = EPERM;
    return -1;
}

int ClothesFilesystem::symlink(String target, String linkpath)
{
    (void)target;
    (void)linkpath;
    errno = EPERM;
    return -1;
}

int ClothesFilesystem::rename(String oldpath, String newpath)
{
    (void)oldpath;
    (void)newpath;
    errno = EPERM;
    return -1;
}

int ClothesFilesystem::unlink(String path)
{
    (void)path;
    errno = EPERM;
    return -1;
}

int ClothesFilesystem::stat(int fd, struct stat *st)
{
    (void)fd;
    (void)st;
    return -1;
}

int ClothesFilesystem::flush(int fd)
{
    (void)fd;
    return 0;
}

Dir *ClothesFilesystem::opendir(String name)
{
    (void)name;
    return NULL;
}

Dirent *ClothesFilesystem::readdir(Dir *dir)
{
    (void)dir;
    return NULL;
}

void ClothesFilesystem::rewinddir(Dir *dir)
{
    (void)dir;
}

int ClothesFilesystem::closedir(Dirent *dir)
{
    (void)dir;
    return -1;
}

int ClothesFilesystem::mkdir(String path, mode_t mode)
{
    (void)path;
    (void)mode;
    errno = EPERM;
    return -1;
}

int ClothesFilesystem::rmdir(String path)
{
    (void)path;
    errno = EPERM;
    return -1;
}
