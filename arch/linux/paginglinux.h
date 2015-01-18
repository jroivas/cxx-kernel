#ifndef PAGING_LINUX_H
#define PAGING_LINUX_H

#include "types.h"
#include "paging.h"

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024
#define PAGE_CNT 1024
#define PAGING_SIZE (sizeof(ptr32_t)*PAGE_CNT) // A little bit more portable
#define PAGE_SIZE PAGING_SIZE

#define PAGING_MAP_USED    (1)
#define PAGING_MAP_RW      (1<<1)
#define PAGING_MAP_USER    (1<<2)
#define PAGING_MAP_R0      (PAGING_MAP_USED|PAGING_MAP_RW)
#define PAGING_MAP_R2      (PAGING_MAP_USED|PAGING_MAP_RW|PAGING_MAP_USER)


class Page
{
public:
    Page();
};

void paging_mmap_init(MultibootInfo *info);

#endif
