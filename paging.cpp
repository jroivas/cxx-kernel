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
	//VideoX86 v;
	ptr_val_t pos = 0;
	while (cnt>0) {
		pos = 0;
		if (_d->map(NULL, &pos, PAGING_MAP_R0)) {
			cnt--;
			//v.printf("Mem reserve: %x   \n",pos);
			if (tmp==NULL && pos!=0) tmp=(void*)pos;
		} else {
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
/*
	unsigned short *atmp = (unsigned short *)(0xB8000);
	atmp+=1;
*/
	//*atmp = 0x1256; //V
	_d->lockStatic();
	//*atmp = 0x1257; //W

	_d->pageAlign(PAGE_SIZE);
	//*atmp = 0x1258; //X

	ptr_val_t tmp = (ptr_val_t)_d->freePageAddress();
	//*atmp = 0x1259; //Y
/*
	if (tmp==0) {
		*atmp = 0x1259; //Y
	}
*/
	if (phys!=NULL) *phys = tmp;
	//*atmp = 0x3759; //Y
	_d->incFreePageAddress(size);
	//*atmp = 0x125a; //Z

	_d->unlockStatic();
	//*atmp = 0x375a; //Z
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
