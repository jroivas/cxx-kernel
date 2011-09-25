#ifndef PAGING_H
#define PAGING_H
#include "mutex.h"
#include "types.h"

struct ElfSectionTable
{
        unsigned long num;
        unsigned long size;
        unsigned long addr;
        unsigned long shndx;
};

/* The Multiboot information. */
struct MultibootInfo
{
        unsigned long flags;
        unsigned long mem_lower;
        unsigned long mem_upper;
        unsigned long boot_device;
        unsigned long cmdline;
        unsigned long mods_count;
        unsigned long mods_addr;
	ElfSectionTable elf_sec;
        unsigned long mmap_length;
        unsigned long mmap_addr;
};

class Paging
{
public:
	Paging();
	void *alloc(size_t cnt);
	void free(void *ptr, size_t cnt);
	
private:
	bool map(void *phys, void *virt, unsigned int flags);
	void *unmap(void *ptr);
	void lock();
	void unlock();
	void *getPage();
	void freePage(void *ptr);
	Mutex m;
	Mutex m_alloc;
};


void paging_init(MultibootInfo *map);
void *paging_alloc(unsigned int cnt);

#endif
