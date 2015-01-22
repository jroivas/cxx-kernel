#include <unistd.h>

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
    (void)pathname;
    (void)buf;
    (void)bufsiz;
    return 0;
}

char *getcwd(char *buf, size_t size)
{
    (void)buf;
    (void)size;
    return NULL;
}
