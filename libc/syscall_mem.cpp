#include "syscall_mem.h"
#include <platform.h>
#include <string.hh>
#include <mm.h>
#include <stdio.h>
#include <sys/mman.h>

void *syscall_mmap(void *addr, size_t length, int prot,
                    int flags, int fd, off_t pgoffset)
{
    (void)addr;
    (void)length;
    (void)prot;
    (void)flags;
    (void)fd;
    (void)pgoffset;

    if (addr != nullptr) {
        printf("ERROR: mmap, non-NULL addr not supported!\n");
        return nullptr;
    }
    if (fd >= 0) {
        printf("ERROR: mmap of files not supported!\n");
        return nullptr;
    }
    if ((flags & MAP_ANONYMOUS)) {
        /* Just perform alloc */
        return calloc(1, length);
    }
    if (pgoffset > 0) {
        printf("ERROR: mmap offset not supported! %lld\n", pgoffset);
        return nullptr;
    }

    printf("mmap: %x %d %d %d %d %d\n", addr, length, prot, flags, fd, pgoffset);

    // Simplest case
    void *res = calloc(1, length);
    return res;
}

long syscall_mmap_wrap(void *addr, size_t length, int prot,
                    int flags, int fd, off_t pgoffset)
{
    return (long)syscall_mmap(addr, length, prot, flags, fd, pgoffset);
}
