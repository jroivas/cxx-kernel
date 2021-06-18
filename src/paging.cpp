#include "paging.h"
#include "types.h"
#include "arch/platform.h"

//Mutex for locking
ptr32_val_t __page_mapping_alloc_mutex = 0;

class PagingPrivateLocker {
public:
    PagingPrivateLocker(PagingPrivate *p) : m_p(p) {
        m_p->lock();
    }
    ~PagingPrivateLocker() {
        m_p->unlock();
    }
private:
    PagingPrivate *m_p;
};


/* Oh, we have a singleton of paging private! */
static PagingPrivate __paging_private;

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
    PagingPrivateLocker lock(_d);

    _d->init(platformData);
}

/* Allocate pages */
void *Paging::alloc(size_t cnt, unsigned int align, Alloc do_map)
{
    PagingPrivateLocker lock(_d);

    /* We need proper locking because _d is a singleton.
     * This of course means only one alloc is allowed at time.
     */
    (void)align;
    (void)do_map;
    void *res = nullptr;
    ptr_val_t pos = 0;
#if 0
    ptr_val_t pp = 0;
    while (cnt>0) {
        pos = 0;
        if (_d->map(&pos, PAGING_MAP_R0, 1)) {
            cnt--;
            if (res==nullptr && pos!=0) {
                res=(void*)pos;
                pp = pos;
            } else {
                pp += PAGE_SIZE;
                if (pp!=pos) {
                    Platform::video()->printf("DISCONTINUATION: %x %x\n",pp,pos);
                    while(1);
                }
            }
        } else {
            res = nullptr;
            break;
        }
#else
    if (_d->map(&pos, PAGING_MAP_R0, cnt)) {
        res = (void*)pos;
#endif
    } else {
        res = nullptr;
    }

    return res;
}

void *Paging::allocStatic(size_t size, ptr_t phys)
{
    _d->lockStatic();

    _d->pageAlign(PAGE_SIZE);
    // Align for boundary
    //_d->pageAlign(sizeof(long long int));

    ptr_val_t tmp = (ptr_val_t)_d->freePageAddress();

    if (phys != nullptr) {
        *phys = tmp;
    }

    _d->incFreePageAddress(size);

    _d->unlockStatic();
    return (void*)tmp;
}

/* Free allocated pages */
void Paging::free(void *ptr, size_t cnt)
{
    PagingPrivateLocker lock(_d);

    while (cnt > 0) {
        _d->freePage(_d->unmap(ptr));
        ptr = (void*)((ptr32_val_t)ptr + PAGE_SIZE);
        --cnt;
    }
}

void Paging::map(void *phys, void *virt, unsigned int flags)
{
    PagingPrivateLocker lock(_d);

    if (!_d->mapPhys(phys, (ptr_t)virt, flags)) {
        if (virt != nullptr) {
            *(ptr_val_t*)virt = 0;
        }
    }
}

void *Paging::getPage()
{
    PagingPrivateLocker lock(_d);

    return _d->getPage();
}

bool Paging::identityMap(ptr_val_t addr)
{
    PagingPrivateLocker lock(_d);

    return _d->identityMap(addr, PagingPrivate::MapPageKernel, PagingPrivate::MapPageRW);
}

void Paging::enablePagingSmp()
{
    PagingPrivateLocker lock(_d);

    _d->enablePagingSmp();
}

void paging_init(MultibootInfo *info)
{
    paging_mmap_init(info);
}

void pagingMapKernel(ptr_val_t addr)
{
    Paging p;

    p.identityMap(addr);
}

void pagingEnableSmp()
{
    Paging p;

    p.enablePagingSmp();
}
