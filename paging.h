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

struct MultibootInfo;

/* The real paging class which does the dirty job, implement arch specific*/
class PagingPrivate
{
public:
	PagingPrivate();
	bool map(void *phys, void *virt, unsigned int flags);
	void *unmap(void *ptr);
	void *getPage();
	void freePage(void *ptr);
	void lock();
	void unlock();
	ptr8_t freePageAddress();
	void incFreePageAddress(ptr_val_t size);
	ptr_val_t memSize();

private:
	Mutex m;
};

class Paging
{
public:
	enum Alloc {
		PagingAllocNormal = 0,
		PagingAllocDontMap = 1
	};

	Paging();
	void *alloc(size_t cnt, unsigned int align=0, Alloc do_map = PagingAllocNormal);
	void free(void *ptr, size_t cnt);
	void lock();
	void unlock();
#if 0
	void *mapPhys(void* phys, unsigned int length);
	void unmapPhys(void* phys, unsigned int length);
#endif
	void map(void *phys, void *virt, unsigned int flags);
	void *getPage();
	
private:
	PagingPrivate *_d;
	Mutex m;
};


void paging_init(MultibootInfo *map);

#endif
