#ifndef __UNISTD_H
#define __UNISTD_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
char *getcwd(char *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif
