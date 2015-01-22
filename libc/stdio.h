#ifndef __STDIO_H
#define __STDIO_H

#include <types.h>
#include <sys/stat.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EOF (-1)

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

// FIXME
#define BUFSIZ 1024

typedef void FILE;

extern void* stdin;
extern void* stdout;
extern void* stderr;

int printf(const char *fmt, ...);
int fprintf(FILE *__restrict __stream, const char *fmt, ...);
int sprintf(char *str, const char *fmt, ...);
int snprintf(char *str, size_t size, const char *fmt, ...);
int vsprintf(char *str, const char *fmt, va_list ap);
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);

FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fd, const char *mode);
//FILE *freopen(const char *path, const char *mode, FILE *stream);
int fclose(FILE *fp);

//int open(const char *pathname, int flags, mode_t mode);
int open(const char *pathname, int flags);
int close(int fd);

char *fgets(char *s, int size, FILE *stream);
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int getc(FILE *stream);
int ungetc(int c, FILE *stream);

int feof(FILE *stream);
int fflush(FILE *stream);

void clearerr(FILE *stream);
int ferror(FILE *stream);

int isatty(int fd);
int fileno(FILE *stream);

long ftell(FILE *stream);
off_t lseek(int fd, off_t offset, int whence);

#ifdef __cplusplus
}
#endif

#endif
