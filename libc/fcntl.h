#ifndef __FCNTL_H
#define __FCNTL_H

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    O_RDONLY = 0,
    O_WRONLY = 1,
    O_RDWR = 2,
    O_CREAT = 0100,
    O_EXCL = 0200,
    O_NOCTTY = 0400,
    O_TRUNC = 01000,
    O_APPEND = 02000,
    O_NONBLOCK = 04000,
    O_SYNC = 010000,
    O_ASYNC = 020000
};

enum
{
    F_DUPFD,
    F_GETFD,
    F_SETFD,
    F_GETFL,
    F_SETFL
};

#define FD_CLOEXEC 1

int fcntl(int fd, int cmd, ...);

#ifdef __cplusplus
}
#endif

#endif
