#ifndef __USTARFS_HH
#define __USTARFS_HH

#ifdef LINUX_BUILD
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#else
#include <string.hh>
#include <platform.h>
#endif

#include <fs/filesystem.hh>

#ifdef USE_CUSTOM_STRING
#include <string.hh>
#define STD_STRING_TYPE String
#else
#include <string>
#define STD_STRING_TYPE std::string
#endif
static const uint32_t FS_BLOCKSIZE = 512;

class Ustar
{
public:
    enum {
        TYPE_INVALID,
        TYPE_FILE,
        TYPE_HARD_LINK,
        TYPE_SYM_LINK,
        TYPE_CHAR_DEV,
        TYPE_BLOCK_DEV,
        TYPE_DIR,
        TYPE_FIFO,
    };

    class Iterator {
        friend class Ustar;
    public:
        Iterator()
            : m_ok(false),
            m_block(0),
            m_index(0),
            m_pos(0),
            m_data_block(0),
            m_data_index(0),
            m_fs(nullptr),
            m_parent(nullptr),
            m_data(nullptr),
            m_content(nullptr)
        {
        }
        Iterator(uint32_t blk, uint32_t index)
            : m_ok(false),
            m_block(blk),
            m_index(index),
            m_pos(0),
            m_data_block(0),
            m_data_index(0),
            m_fs(nullptr),
            m_parent(nullptr),
            m_data(nullptr),
            m_content(nullptr)
        {
        }
        ~Iterator() {
            m_ok = false;
#if 0
            if (m_parent != nullptr) {
                delete[] m_parent;
            }
#endif
            if (m_data != nullptr) {
                delete[] m_data;
            }
            if (m_content != nullptr) {
                delete[] m_content;
            }
            m_parent = nullptr;
            m_data = nullptr;
            m_content = nullptr;
        }
        Iterator(const Iterator &another)
            : m_ok(false),
            m_pos(0),
            m_data_block(0),
            m_data_index(0),
            m_fs(nullptr),
            m_parent(nullptr),
            m_data(nullptr)
        {
            assign(another);
        }

        Iterator &operator=(const Iterator &another)
        {
            assign(another);
            return *this;
        }

        void assign(const Iterator &another)
        {
            m_fs = another.m_fs;
            m_block = another.m_block;
            m_index = another.m_index;
            m_data_block = another.m_data_block;
            m_data_index = another.m_data_index;
            m_size = another.m_size;
            m_ok = another.m_ok;

            if (m_fs != nullptr) {
                m_parent = new uint8_t[m_fs->m_blocksize];
                m_data = new uint8_t[m_fs->m_blocksize];
                m_content = new uint8_t[m_fs->m_blocksize];

#ifdef LINUX_BUILD
                if (another.m_parent != nullptr) {
                    memmove(m_parent, another.m_parent, m_fs->m_blocksize);
                }
                if (another.m_data != nullptr) {
                    memmove(m_data, another.m_data, m_fs->m_blocksize);
                }
                if (another.m_content != nullptr) {
                    memmove(m_content, another.m_content, m_fs->m_blocksize);
                }
#else
                if (another.m_parent != nullptr) {
                    Mem::move(m_parent, another.m_parent, m_fs->m_blocksize);
                }
                if (another.m_data != nullptr) {
                    Mem::move(m_data, another.m_data, m_fs->m_blocksize);
                }
                if (another.m_content != nullptr) {
                    Mem::move(m_content, another.m_content, m_fs->m_blocksize);
                }
#endif
            }
        }

        bool next();
        inline bool ok() const
        {
            return m_ok;
        }
        inline uint8_t *data()
        {
            return m_data;
        }
        STD_STRING_TYPE name();
        uint32_t nameLen();
        uint64_t size();
        uint8_t type() const;
        ssize_t read(uint8_t *buf, uint64_t cnt);
        uint32_t block() const
        {
            return m_block;
        }
        inline uint32_t blockSize() const
        {
            return m_fs->blockSize();
        }

    protected:
        bool getCurrent();

        bool m_ok;
        uint32_t m_block;
        uint32_t m_index;
        uint64_t m_pos;
        uint32_t m_data_block;
        uint32_t m_data_index;
        uint64_t m_size;

        Ustar *m_fs;
        uint8_t *m_parent;
        uint8_t *m_data;
        uint8_t *m_content;
    };

    Ustar();
    ~Ustar();

    void setPhysical(FilesystemPhys *phys);
    inline uint32_t blockSize() const
    {
        return m_blocksize;
    }

    bool detect();
    Ustar::Iterator list(
        uint32_t parent);
    uint64_t size() const {
        return m_phys ? m_phys->size() : 0;
    }
    uint32_t getDevId() const
    {
        return (uint32_t)m_phys;
    }

protected:
    uint32_t formatBlocks();
    bool getBlock(uint32_t index, uint8_t *buffer);
    void clearBuffer(uint8_t *buf, uint32_t size);

    bool verifySectorSize() const;

    FilesystemPhys *m_phys;
    uint32_t m_blocksize;
    uint32_t m_blocks;
    uint32_t m_freechain;
    uint32_t m_block_in_sectors;
};

class UstarFilesystem : public Filesystem
{
public:
    UstarFilesystem();
    UstarFilesystem(const UstarFilesystem &fs);
    virtual ~UstarFilesystem() {}

    virtual inline const String type() const
    {
        return String("Ustar");
    }
    virtual void setPhysical(FilesystemPhys *phys)
    {
        m_ustar->setPhysical(phys);
        m_ustar->detect();
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
    Ustar::Iterator findFile(
        String path,
        uint32_t part=0,
        uint32_t parent=0);
    String m_mountpoint;
    String m_opts;
    Ustar *m_ustar;
};
#endif
