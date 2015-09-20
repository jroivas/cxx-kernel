#pragma once

#include <stdio.h>
#include <types.h>

void *syscall_mmap(void *addr, size_t length, int prot,
                    int flags, int fd, off_t pgoffset);
