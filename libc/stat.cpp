#include <sys/stat.h>
#include <errno.h>

int stat(const char *pathname, struct stat *buf)
{
    if (buf == NULL) {
        //FIXME
        errno = ENOENT;
        return -1;
    }

    (void)pathname;
    (void)buf;
    return 0;
}
