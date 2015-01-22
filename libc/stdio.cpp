#include "stdio.h"
#include "video.h"
#include <stdarg.h>
#include <errno.h>
#include <mm.h>

class IO_FILE
{
public:
    IO_FILE()
        : fd(0),
        errno(0),
        pos(0),
        mode(0),
        data(NULL)
    {
    }
    IO_FILE(int _fd)
        : fd(_fd),
        errno(0),
        pos(0),
        mode(0),
        data(NULL)
    {
    }

    int fd;
    int errno;
    int pos;
    int mode;
    char *data;
};

static int _fd_max = 3;

static IO_FILE _stdio_streams[] = {
    IO_FILE(0),
    IO_FILE(1),
    IO_FILE(2)
};
//FIXME
void *stdin = (void*)&_stdio_streams[0];
void *stdout = (void*)&_stdio_streams[1];
void *stderr = (void*)&_stdio_streams[2];

#define FP(x) ((IO_FILE*)(x))

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

void clearerr(FILE *stream)
{
    if (stream == NULL) return;
    FP(stream)->errno = 0;
}

int ferror(FILE *stream)
{
    if (stream == NULL) return -1;
    return FP(stream)->errno;
}

int getc(FILE *stream)
{
    // FIXME
    unsigned char res = FP(stream)->data[FP(stream)->pos];
    ++FP(stream)->pos;
    return res;
}

int ungetc(int c, FILE *stream)
{
    // FIXME
    --FP(stream)->pos;
    FP(stream)->data[FP(stream)->pos] = (unsigned char)c;
    return c;
}

char *fgets(char *s, int size, FILE *stream)
{
    //FIXME
    (void)s;
    (void)size;
    (void)stream;

    return NULL;
}

int feof(FILE *stream)
{
    if (stream == NULL) return 0;
    return FP(stream)->errno == EOF;
}

int fflush(FILE *stream)
{
    //TODO
    (void)stream;
    return 0;
}

int isatty(int fd)
{
    //FIXME
    (void)fd;
    return 0;
}

int fileno(FILE *stream)
{
    if (stream == NULL) return 0;
    return FP(stream)->fd;
}

long ftell(FILE *stream)
{
    return FP(stream)->pos;
}

off_t lseek(int fd, off_t offset, int whence)
{
    //TODO
    (void)fd;
    (void)offset;
    (void)whence;
    return (off_t)-1;
}

FILE *fopen(const char *path, const char *mode)
{
    IO_FILE *tmp = new IO_FILE();
    tmp->fd = ++_fd_max;

    //TODO
    (void)path;
    (void)mode;

    return tmp;
}

FILE *fdopen(int fd, const char *mode)
{
    IO_FILE *tmp = new IO_FILE();
    tmp->fd = fd;

    //TODO
    (void)mode;

    return tmp;
}

int fclose(FILE *fp)
{
    // FIXME
    FP(fp)->errno = EOF;
    return 0;
}

int sprintf(char *str, const char *fmt, ...)
{
    (void)str;
    (void)fmt;
    return 0;
}

int snprintf(char *str, size_t size, const char *fmt, ...)
{
    (void)str;
    (void)size;
    (void)fmt;
    return 0;
}

int vsprintf(char *str, const char *fmt, va_list ap)
{
    (void)str;
    (void)fmt;
    (void)ap;
    return 0;
}

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap)
{
    (void)str;
    (void)size;
    (void)fmt;
    (void)ap;
    return 0;
}

int sprintf(const char *fmt, ...)
{
    (void)fmt;
    return 0;
}

int fputc(int c, FILE *stream)
{
    (void)c;
    (void)stream;
    return 1;
}

int fputs(const char *s, FILE *stream)
{
    if (s == NULL) return 0;

    int cnt = 0;
    while (*s != 0) {
        int res = fputc(*s, stream);
        if (res == 0) break;
        res += cnt;
        ++s;
    }

    return cnt;
}

//int open(const char *pathname, int flags, mode_t mode)
int open(const char *pathname, int flags)
{
    (void)pathname;
    (void)flags;
    //(void)mode;

        //FIXME
    return 6;
}

int close(int fd)
{
    (void)fd;
    return 0;
}
