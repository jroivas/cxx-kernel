#include "string.h"
#include "string.hh"
#include "mm.h"

size_t strlen(const char *s)
{
    return String::length(s);
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    while (n) {
        char diff = *p1 - *p2;
        if (diff > 0) return 1;
        else if (diff < 0) return -1;

        ++p1;
        ++p2;
        --n;
    }

    return 0;
}

int strcmp(const char *s1, const char *s2)
{
    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);
    size_t min_len = l1 < l2 ? l1 : l2;

    int res = memcmp(s1, s2, min_len);
    if (res != 0) {
        return res;
    }

    if (l1 > l2) {
        return 1;
    } else if (l1 < l2) {
        return -1;
    }

    return 0;
}


int strncmp(const char *s1, const char *s2, size_t n)
{
    return memcmp(s1, s2, n);
}

char *strdup(const char *s)
{
    size_t len = String::length(s);

    char *res = (char*)MM::instance()->alloc(len + 1);
    if (res != NULL) {
        Mem::move(res, s, len);
    }
    res[len] = 0;
    return res;
}

char *strndup(const char *s, size_t n)
{
    size_t len = String::length(s);
    if (len < n) {
        n = len;
    }

    char *res = (char*)MM::instance()->alloc(n + 1);
    if (res != NULL) {
        Mem::move(res, s, n);
    }
    res[n] = 0;
    return res;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    return (char*)Mem::copy(dest, src, n);
}

char *strcpy(char *dest, const char *src)
{
    size_t n = String::length(src);
    return (char*)Mem::copy(dest, src, n);
}

void *memchr(const void *s, int c, size_t n)
{
    if (s == NULL) return NULL;

    char *p = (char*)s;
    while (n) {
        if (*p == c) return p;
        ++p;
        --n;
    }
    return NULL;
}

void *memrchr(const void *s, int c, size_t n)
{
    if (s == NULL) return NULL;

    char *p = (char*)s;
    p += n;
    while (n) {
        if (*p == c) return p;
        --p;
        --n;
    }
    return NULL;
}

char *strchr(const char *s, int c)
{
    return (char*)memchr(s, c, strlen(s) + 1);
}

char *strrchr(const char *s, int c)
{
    return (char*)memrchr(s, c, strlen(s) + 1);
}


static char *__errmsg = NULL;
char *strerror(int errnum)
{
    (void)errnum;
    if (__errmsg == NULL) {
        __errmsg = strdup("TODO Error message");
    }
    return __errmsg;
}
