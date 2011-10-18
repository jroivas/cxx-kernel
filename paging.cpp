#include "paging.h"
#include "types.h"
#include "arch/platform.h"
//#include "x86.h"

//#define PAGE_CNT 1024
//#define LPOS 0x100000
#define PAGING_START_POS 0x40000000
//#define PAGING_START_POS 0x1000
//#define PAGE_SIZE 4096

//#define PAGING_SIZE (sizeof(ptr32_t)*PAGE_CNT) // A little bit more portable
#define PAGE_SIZE PAGING_SIZE

ptr32_t __page_directory    = (ptr32_t)0xFFFFF000; //TODO
ptr32_t __page_table        = (ptr32_t)0xFFC00000;
ptr8_t  __free_page_address = (ptr8_t)PAGING_START_POS;
ptr8_t      __mem_map       = (ptr8_t)(KERNEL_VIRTUAL+0x500);
ptr32_val_t __mem_size      = 0;


//Mutex for locking mapping and prevent theading/SMP problems
ptr32_val_t __page_mapping_mutex       = 0;
ptr32_val_t __page_mapping_alloc_mutex = 0;

/* Memory mapping structure */
struct MemoryMap
{
        unsigned long size;
        unsigned long base_addr_low;
        unsigned long base_addr_high;
        unsigned long length_low;
        unsigned long length_high;
        unsigned long type;
};

/* Paging class which does the dirty job */
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

private:
	Mutex m;
};

/* Oh, we have a singleton of it! */
PagingPrivate __paging_private;


Paging::Paging()
{
	_d = &__paging_private;
	m = Mutex(&__page_mapping_alloc_mutex);
}

/* User lock, do not use internally */
void Paging::lock()
{
	m.lock();
}

/* User unlock, do not use internally */
void Paging::unlock()
{
	m.unlock();
}

/* Allocate pages */
void *Paging::alloc(size_t cnt, unsigned int align)
{
	/* We need proper locking because _d is a singleton.
	 * This of course means only one alloc is allowed at time.
	 */
	_d->lock();
	//void *tmp = __free_page_address;

	if (align==0) {
		align = PAGE_SIZE;
	}

	/* Always align to proper boundaries */
	ptr8_t tmp = (ptr8_t)__free_page_address;
	while ((ptr32_val_t)tmp & (align - 1)) {
		tmp++;
	}
	__free_page_address = tmp;

	while (cnt>0) {
		if (_d->map(_d->getPage(), __free_page_address, 0x3)) {
			__free_page_address += PAGE_SIZE;
			cnt--;
		} else {
			_d->unlock();
			return NULL;
		}
	}

	_d->unlock();
	return tmp;
}

/* Free allocated pages */
void Paging::free(void *ptr, size_t cnt)
{
	_d->lock();
	while (cnt>0) {
		_d->freePage(_d->unmap(ptr));
		ptr = (void*)((ptr32_val_t)ptr + PAGE_SIZE);
		//ptr += PAGE_SIZE;
		cnt--;
	}
	_d->unlock();
}

/* Map physical page */
void *Paging::mapPhys(void* phys, unsigned int length)
{
	_d->lock();
	unsigned int cc = length;
	ptr8_t virt = (ptr8_t)phys;
	while (cc!=0) {
		//_d->map(phys, __free_page_address, 0x3);
		_d->map(virt, virt, 0x3);
		virt += PAGE_SIZE;
		//__free_page_address += PAGE_SIZE;

		if (cc>=PAGE_SIZE) cc -= PAGE_SIZE;
		else break;
	}
	_d->unlock();
	//return virt;
	return phys;
}

void Paging::unmapPhys(void* phys, unsigned int length)
{
	unsigned int cnt = length/PAGE_SIZE;
	if (length%length/PAGE_SIZE!=0) cnt++;
	free(phys, cnt);
}

PagingPrivate::PagingPrivate()
{
	m = Mutex(&__page_mapping_mutex);
}

void PagingPrivate::lock()
{
	// Doing it hard way...
	//cli();
	m.lock();
}

void PagingPrivate::unlock()
{
	m.unlock();
	//sti();
}

/* Get a free physical page */
void *PagingPrivate::getPage()
{
	static unsigned int count = 0;
	unsigned char t = 0;

	while (1) {
		// Found a free page
		if ((__mem_map[count] & (1<<t)) != 0) {
			unsigned char i = 0;

			// Mark it used
			__mem_map[count] ^= (1<<t);
			while (t>0) {
				i++;
				t--;
			}
			return (void *)(((count*8)+i)*PAGE_SIZE);
		}

		// Last bit, start all over
		if (t==7) {
			t = 0;
			count++;
		} else {
			t++;
		}

		// Check for end of memory
		if (count*PAGE_SIZE >= __mem_size) {
			return NULL;
		}
	}

	return NULL;
}

/* Free physical page corresponding to given virtuall address */
void PagingPrivate::freePage(void *ptr)
{
	ptr32_val_t count = (((ptr32_val_t)ptr) / 0x8000);
        ptr8_val_t  bit   = ((((ptr32_val_t)ptr) / 0x1000) % 8);

	__mem_map[count] |= bit;
}

void Paging::map(void *phys, void *virt, unsigned int flags)
{
	_d->lock();
	_d->map(phys,virt,flags);
	_d->unlock();
}

/* Map physical page to virtual */
bool PagingPrivate::map(void *phys, void *virt, unsigned int flags)
{
	ptr32_val_t  pagedir   = (ptr32_val_t)virt >> 22;
	ptr32_val_t  pagetable = (ptr32_val_t)virt >> 12 & 0x3FF;

	//unsigned long *pd = (unsigned long *) 0xFFFFF000;
	ptr32_t pd = __page_directory;
        if ((pd[pagedir] & 1) != 1) {
                pd[pagedir] = (ptr32_val_t )getPage() | 0x3;
        }

        //ptr32_t pt = (ptr32_t )0xFFC00000 + (0x400 * pagedir);
        //ptr32_t pt = (ptr32_t)((ptr32_val_t)__page_table + (0x400 * pagedir));
        ptr32_t pt = __page_table + (0x400 * pagedir);
        if ((pt[pagetable] & 1) == 1) { //TODO Sharing
		Platform p;
		p.video()->printf("Paging error HALT!\n");
		p.state()->seizeInterrupts();
		p.state()->halt();
/*
                cli();
                hlt();
*/
		return false;
        }

        pt[pagetable] = ((ptr32_val_t )phys) | (flags & 0xFFF) | 0x01;

	return true;
}

/* Unmap memory at ptr */
void *PagingPrivate::unmap(void *ptr)
{
	ptr32_val_t pagedir   = (ptr32_val_t)ptr >> 22;
	ptr32_val_t pagetable = (ptr32_val_t)ptr >> 12 & 0x3FF;
	
        //ptr32_t pt = (ptr32_t)__page_table + (0x400 * pagedir);
        //ptr32_t pt = (ptr32_t)((ptr32_val_t)__page_table + (0x400 * pagedir));
        ptr32_t pt = __page_table + (0x400 * pagedir);
        if ((pt[pagetable] & 1) == 0) {
		Platform p;
		p.video()->printf("Paging error!\n");
		return NULL;
	}

	pt[pagetable] &= 0xFFFFFFFE;
	return (void *) (pt[pagetable] & 0xFFFFF000);
}

void paging_mmap_init(MultibootInfo *info)
{
	MemoryMap *mmap = (MemoryMap*)(info->mmap_addr + KERNEL_VIRTUAL);
	ptr32_val_t  info_end = info->mmap_addr + info->mmap_length + KERNEL_VIRTUAL;

	while ((ptr32_val_t )(mmap) + mmap->size < info_end) {
		if ((mmap->base_addr_low + mmap->length_low) > __mem_size) {
			__mem_size = mmap->base_addr_low + mmap->length_low;
		}

		unsigned long addr = mmap->base_addr_low / 0x1000;
		unsigned long limit = mmap->length_low / 0x1000;

		while (addr<0x120 && limit>0) {
			addr++;
			limit--;
		}

		if (mmap->type == 1) {
			while (limit>0) {
				__mem_map[addr/8] |= 1 << addr % 8;
				addr++;
				limit--;
			}
		}
		else if (mmap->type == 2 || mmap->type == 3) {
			//Skip
		}
		else {
			return;
		}
		mmap = (MemoryMap *)(((ptr32_val_t )mmap) + mmap->size + sizeof(ptr32_val_t ));
	}
}

void paging_init(MultibootInfo *info)
{
	paging_mmap_init(info);
}

extern "C" void *mapPhys(void* phys, unsigned int length)
{
	Paging p;
	void *res;
	p.lock();
	res = p.mapPhys(phys,length);
	p.unlock();
	return res;
}

extern "C" void unmapPhys(void *phys, unsigned int length)
{
	Paging p;
	p.lock();
	p.unmapPhys(phys, length);
	p.unlock();
}
