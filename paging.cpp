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

	(void)align;
	(void)do_map;
	void *tmp = NULL;
	ptr_val_t pos = 0;
	while (cnt>0) {
		pos = 0;
		if (_d->map(&pos, PAGING_MAP_R0)) {
			cnt--;
			//v.printf("Mem reserve: %x   \n",pos);
			if (tmp==NULL && pos!=0) tmp=(void*)pos;
		} else {
/*
			unsigned short *atmp = (unsigned short *)(0xB8000);
			*atmp = 0x1745;
			while(1);
*/
			tmp = NULL;
			break;
		}
	}

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
	if (!_d->mapPhys(phys, (ptr_t)virt, flags)) {
		unsigned short *vid = (unsigned short *)(0xB8000);
		*vid = 0x814a; //J
		//while(1);
		//if (virt!=NULL) *(ptr_val_t*)virt = NULL;
	}
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
