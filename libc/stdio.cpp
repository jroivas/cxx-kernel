#include "stdio.h"
#include "video.h"
#include <stdarg.h>

int printf(const char *fmt, ...)
{
    Video *video = Video::get();

    va_list al;
    va_start(al, fmt);
    int cnt = video->vprintf(fmt, al);
    va_end(al);

    return cnt;
}
