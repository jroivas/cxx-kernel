#include "paginglinux.h"
#include <sys/mman.h>

ptr32_val_t __page_mapping_mutex          = 0;
ptr32_val_t __page_mapping_static_mutex   = 0;
uint8_t *__free_page_address = nullptr;
uint8_t *__free_page_chunk = nullptr;
uint32_t __free_page_chunk_size = 0;

PagingPrivate::PagingPrivate()
{
    m.assign(&__page_mapping_mutex);
    m_static.assign(&__page_mapping_static_mutex);
    data = nullptr;
    pageCnt = 0;
    is_ok = false;
    __free_page_chunk_size = 1000;
    __free_page_chunk = (uint8_t*)mmap(nullptr, __free_page_chunk_size*4096, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    __free_page_address = __free_page_chunk;
}

PagingPrivate::~PagingPrivate()
{
}

bool PagingPrivate::init(void *platformData)
{
    (void)platformData;
    return true;
}

void PagingPrivate::lock()
{
    m.lock();
}

void PagingPrivate::unlock()
{
    m.unlock();
}

void PagingPrivate::lockStatic()
{
    m_static.lock();
}

void PagingPrivate::unlockStatic()
{
    m_static.unlock();
}

bool PagingPrivate::identityMapFrame(Page *p, ptr_val_t addr, MapType type, MapPermissions perms)
{
    (void)p;
    (void)addr;
    (void)type;
    (void)perms;
    return false;
}

bool PagingPrivate::mapFrame(Page *p, MapType type, MapPermissions perms)
{
    (void)p;
    (void)type;
    (void)perms;
    return false;
}

void *PagingPrivate::getPage()
{
    return (void*)mmap(nullptr, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void PagingPrivate::freePage(void *ptr)
{
    munmap(ptr, 4096);
}

bool PagingPrivate::mapPhys(void *phys, ptr_t virt, unsigned int flags)
{
    (void)phys;
    return map(virt, flags);
}

bool PagingPrivate::map(ptr_t virt, unsigned int flags, unsigned int cnt)
{
    (void)flags;
    ptr_t block = (ptr_t)mmap(nullptr, PAGE_SIZE*cnt, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block==nullptr) return false;
    if (virt!=nullptr) {
            *virt = (ptr_val_t)block;
    }
    return true;
}

void *PagingPrivate::unmap(void *ptr)
{
    munmap(ptr, 4096);
    return nullptr;
}

ptr8_t PagingPrivate::freePageAddress()
{
    return __free_page_address;
}

void PagingPrivate::incFreePageAddress(ptr_val_t size)
{
    __free_page_address += size;
}

void PagingPrivate::pageAlign(ptr_val_t align)
{
    (void)align;
}

ptr_val_t PagingPrivate::memSize()
{
    return 10000000;
}

void paging_mmap_init(MultibootInfo *info)
{
    /* Initialize paging */
    Paging p;
    p.init((void*)info);
}
