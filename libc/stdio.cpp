#include "stdio.h"
#include "video.h"
#include <stdarg.h>

struct _IO_FILE
{
    int fd;
};

//FIXME
static _IO_FILE _stdio_streams[] = {
    {0},
    {1},
    {2}
};

struct _IO_FILE *stdin = _stdio_streams;
struct _IO_FILE *stdout = _stdio_streams + 1;
struct _IO_FILE *stderr = _stdio_streams + 2;

int printf(const char *fmt, ...)
{
    Video *video = Video::get();

    va_list al;

    va_start(al, fmt);
    int cnt = video->vprintf(fmt, al);

    va_end(al);

    return cnt;
}

int fprintf(FILE *__restrict __stream, const char *fmt, ...)
{
    (void)__stream;
    Video *video = Video::get();

    va_list al;

    va_start(al, fmt);
    int cnt = video->vprintf(fmt, al);

    va_end(al);

    return cnt;
}
