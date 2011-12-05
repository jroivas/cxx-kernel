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
	m.assign(&__page_mapping_alloc_mutex);
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

bool Paging::isOk()
{
	return _d->isOk();
}

void Paging::init(void *platformData)
{
	_d->lock();
	_d->init(platformData);
	_d->unlock();
}

/* Allocate pages */
void *Paging::alloc(size_t cnt, unsigned int align, Alloc do_map)
{
	/* We need proper locking because _d is a singleton.
	 * This of course means only one alloc is allowed at time.
	 */
	_d->lock();

#if 0
	if (align==0) {
		align = PAGE_SIZE;
	}

	_d->pageAlign(align);
	void *tmp = _d->freePageAddress();

	while (cnt>0) {
		if (do_map == PagingAllocDontMap) {
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
	while (cnt>0) {
		if (do_map == PagingAllocDontMap) {
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
#endif
	(void)align;
	(void)do_map;
	void *tmp = NULL;
	ptr_val_t pos = 0;
	while (cnt>0) {
		pos = 0;
		if (_d->map(NULL, &pos, PAGING_MAP_R0)) {
			cnt--;
			//v.printf("Mem reserve: %x   \n",pos);
			if (tmp==NULL && pos!=0) tmp=(void*)pos;
		} else {
			unsigned short *atmp = (unsigned short *)(0xB82B0);
			*atmp = 0x1745;
			while(1);
			tmp = NULL;
			break;
		}
	}
#if 0
	unsigned short *atmp = (unsigned short *)(0xB82B0);
	//*tmp = 0x1744; //D
	uint32_t t = (uint32_t)tmp;
	*(atmp++) = 0x4741;
	atmp++;
	if (t==0) {
		*(atmp++) = 0x5730;
	}
	while (t>0) {
		*(atmp++) = 0x5730+(t%10);
		t/=10;
	}
#endif

	//while(1);

	_d->unlock();

	return tmp;
}

void *Paging::allocStatic(size_t size, ptr_t phys)
{
	_d->lockStatic();

	_d->pageAlign(PAGE_SIZE);

	ptr_val_t tmp = (ptr_val_t)_d->freePageAddress();
	if (phys!=NULL) *phys = tmp;
	_d->incFreePageAddress(size);

	_d->unlockStatic();
	return (void*)tmp;
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

void Paging::map(void *phys, void *virt, unsigned int flags)
{
	_d->lock();
	(void)phys;
	(void)virt;
	(void)flags;
	_d->unlock();
}

void *Paging::getPage()
{
	_d->lock();
	void *p = _d->getPage();
	_d->unlock();
	return p;
}

void paging_init(MultibootInfo *info)
{
	paging_mmap_init(info);
}
