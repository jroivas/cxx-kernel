#include "paging.h"
#include "types.h"
#include "arch/platform.h"

//Mutex for locking
ptr32_val_t __page_mapping_alloc_mutex = 0;

/* Oh, we have a singleton of paging private! */
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
void *Paging::alloc(size_t cnt, unsigned int align, Alloc do_map)
{
	/* We need proper locking because _d is a singleton.
	 * This of course means only one alloc is allowed at time.
	 */
	_d->lock();

	if (align==0) {
		align = PAGE_SIZE;
	}

	/* Always align to proper boundaries */
	ptr8_t tmp = _d->freePageAddress();
	while (((ptr32_val_t)tmp & (align-1))!=0) {
		tmp++;
		_d->incFreePageAddress(1);
	}

	while (cnt>0) {
		if (do_map == PagingAllocDontMap) {
			//__free_page_address += PAGE_SIZE;
			_d->incFreePageAddress(PAGE_SIZE);
			cnt--;
		} else {
			void *page = _d->getPage();
			if (page==NULL) return NULL;
			if (_d->map(page, _d->freePageAddress(), PAGING_MAP_R0)) {
				_d->incFreePageAddress(PAGE_SIZE);
				cnt--;
			} else {
				_d->unlock();
				return NULL;
			}
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

#if 0
/* Map physical page */
void *Paging::mapPhys(void* phys, unsigned int length)
{
#if 0
	_d->lock();
	unsigned int cc = length;
	ptr8_t virt = (ptr8_t)phys;
	while (cc!=0) {
		//_d->map(phys, __free_page_address, PAGING_MAP_R0);
		_d->map(virt, virt, PAGING_MAP_R0);
		virt += PAGE_SIZE;
		//__free_page_address += PAGE_SIZE;

		if (cc>=PAGE_SIZE) cc -= PAGE_SIZE;
		else break;
	}
	_d->unlock();
	//return virt;
	return phys;
#else
	(void)phys;
	(void)length;
	return NULL;
#endif
}

void Paging::unmapPhys(void* phys, unsigned int length)
{
#if 0
	unsigned int cnt = length/PAGE_SIZE;
	if (length%length/PAGE_SIZE!=0) cnt++;
	free(phys, cnt);
#else
	(void)phys;
	(void)length;
#endif
}
#endif

void paging_init(MultibootInfo *info)
{
	paging_mmap_init(info);
}

#if 0
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
#endif
