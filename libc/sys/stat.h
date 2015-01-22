#ifndef __STAT_H
#define __STAT_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    time_t st_atime;
    time_t st_atime_nsec;
    time_t st_mtime;
    time_t st_mtime_nsec;
    time_t st_ctime;
    time_t st_ctime_nsec;
};

enum {
    S_TYPE_IFDIR = 0040000,
    S_TYPE_IFCHR = 0020000,
    S_TYPE_IFBLK = 0060000,
    S_TYPE_IFREG = 0100000,
    S_TYPE_IFIFO = 0010000,
    S_TYPE_IFLNK = 0120000,
    S_TYPE_IFSCK = 0140000,
    S_TYPE_MASK  = 0170000
};

#define __MODE_CHECK(mode, check) (((mode) & S_TYPE_MASK) == (check))

#define S_IFMT S_TYPE_MASK
#define S_ISREG(mode) __MODE_CHECK(mode, S_TYPE_IFREG)
#define S_ISDIR(mode) __MODE_CHECK(mode, S_TYPE_IFDIR)
#define S_ISCHR(mode) __MODE_CHECK(mode, S_TYPE_IFCHR)
#define S_ISBLK(mode) __MODE_CHECK(mode, S_TYPE_IFBLK)
#define S_ISFIFO(mode) __MODE_CHECK(mode, S_TYPE_IFIFO)
#define S_ISLNK(mode) __MODE_CHECK(mode, S_TYPE_IFLNK)
#define S_ISSOCK(mode) __MODE_CHECK(mode, S_TYPE_IFSCK)

int stat(const char *pathname, struct stat *buf);

#ifdef __cplusplus
}
#endif

#endif
