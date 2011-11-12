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

class Page;

/* The real paging class which does the dirty job, implement arch specific*/
class PagingPrivate
{
public:
	enum MapType {
		MapPageKernel,
		MapPageUser,
	};
	enum MapPermissions {
		MapPageReadOnly,
		MapPageRW,
	};
	PagingPrivate();
	void init();

	void mapFrame(Page *p, MapType type, MapPermissions perms);
	bool map(void *phys, void *virt, unsigned int flags);

	void *unmap(void *ptr);
	void *getPage();
	void freePage(void *ptr);
	void lock();
	void unlock();
	ptr8_t freePageAddress();
	void incFreePageAddress(ptr_val_t size);
	ptr_val_t memSize();
	void pageAlign(ptr_val_t align);

private:
	Mutex m;
	void *data;
	void *directory;
	uint32_t pageCnt;
};

class Paging
{
public:
	enum Alloc {
		PagingAllocNormal = 0,
		PagingAllocDontMap = 1
	};

	Paging();
	void init();
	void *alloc(size_t cnt, unsigned int align=0, Alloc do_map = PagingAllocNormal);
	void free(void *ptr, size_t cnt);
	void lock();
	void unlock();
	void *allocStatic(size_t size, ptr_t phys);
#if 0
	void *mapPhys(void* phys, unsigned int length);
	void unmapPhys(void* phys, unsigned int length);
#endif
	void map(void *phys, void *virt, unsigned int flags);
	void *getPage();
	bool isOk() { return is_ok; }
	
private:
	PagingPrivate *_d;
	Mutex m;
	bool is_ok;
};


void paging_init(MultibootInfo *map);


#endif
