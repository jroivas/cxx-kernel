#ifndef __STDIO_H
#define __STDIO_H

#ifdef __cplusplus
extern "C" {
#endif
#include <types.h>
#include <sys/stat.h>

struct _IO_FILE;
typedef struct _IO_FILE FILE;

extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;

int printf(const char *fmt, ...);
int fprintf(FILE *__restrict __stream, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
