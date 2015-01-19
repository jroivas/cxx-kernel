#ifndef __ERRNO_H
#define __ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EDOM = 1,
    HUGE_VAL,
    ERANGE,
    EAGAIN,
    EALREADY,
    EINPROGRESS,
    EWOULDBLOCK,
    EPIPE,
    ESHUTDOWN,
    ECHILD,
    ECONNABORTED,
    ECONNREFUSED,
    ECONNRESET,
    EEXIST,
    ENOENT,
    EISDIR,
    ENOTDIR,
    EINTR,
    EACCES,
    EPERM,
    ESRCH,
    ETIMEDOUT,
    ENXIO,
    ELAST
};

extern int errno;

#ifdef __cplusplus
}
#endif

#endif
