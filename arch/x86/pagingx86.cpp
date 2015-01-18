#include "pagingx86.h"
#include "types.h"
#include "x86.h"
#include "bits.h"

//#define PAGE_SIZE PAGING_SIZE
#define PDIR(x) ((PageDir*)x)
#define BITS(x) ((Bits*)x)

#define KERNEL_PRE_POS     0x20000000
#define KERNEL_INIT_SIZE   0x000FFFFF
#define HEAP_START         0x30000000
#define HEAP_END           0x3FFFFFFF
#define USER_HEAP_START    0x40000000
#define USER_HEAP_END      0x4FFFFFFF

static ptr8_t  __free_page_address = (ptr8_t)KERNEL_PRE_POS;
static ptr8_t  __heap_address      = (ptr8_t)HEAP_START;
static ptr8_t  __user_heap_address = (ptr8_t)USER_HEAP_START;
static ptr32_val_t __mem_size      = 0;

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


extern "C" void pagingEnable();
extern "C" void pagingDisable();
extern "C" void pagingDirectoryChange(ptr_t a);

//Mutex for locking mapping and prevent theading/SMP problems
ptr32_val_t __page_mapping_mutex          = 0;
ptr32_val_t __page_mapping_static_mutex   = 0;

extern "C" void copyPhysicalPage(ptr_val_t a, ptr_val_t b);

void Page::copyPhys(Page p)
{
    copyPhysicalPage(getAddress(), p.getAddress());
}

void Page::alloc(PageType type)
{
    if (!isAvail()) return;

    Paging p;
    void *page = p.getPage();
    if (page == NULL) return;

    setAddress((ptr_val_t)page);

    setPresent(true);
    setRw(true);
    if (type == KernelPage) {
        setUserspace(false);
    } else {
        setUserspace(true);
    }
}

PageTable::PageTable()
{
#if 0
    for (unsigned int i=0; i<PAGES_PER_TABLE; i++) {
        pages[i] = Page();
    }
#endif
}

Page *PageTable::get(uint32_t i)
{
    if (i < PAGES_PER_TABLE) return &pages[i];
    return NULL;
}

bool PageTable::copyTo(PageTable *table)
{
    if (table == NULL) return false;

    for (int i = 0; i < PAGES_PER_TABLE; i++) {
        if (pages[i].isAvail()) {
            table->pages[i].alloc();
            pages[i].copyTo(&table->pages[i]);
            pages[i].copyPhys(table->pages[i]);
        }
    }

    return true;
}

extern uint32_t kernel_end;
extern uint32_t my_kernel_end;
PagingPrivate::PagingPrivate()
    : data(NULL),
    pageCnt(0),
    is_ok(false)
{
    m.assign(&__page_mapping_mutex);
    m_static.assign(&__page_mapping_static_mutex);
    __mem_size = 0;

    // Map free space after kernel
    __free_page_address = (ptr8_t)my_kernel_end;
}

PagingPrivate::~PagingPrivate()
{
}

bool PagingPrivate::init(void *platformData)
{
    if (data != NULL) return false;
    is_ok = false;

#if 1
    cli();
    pagingDisable(); //Safety
#endif

    if (platformData != NULL) {
        MultibootInfo *info = (MultibootInfo*)platformData;
        MemoryMap *mmap = (MemoryMap*)(info->mmap_addr);
        ptr32_val_t  info_end = info->mmap_addr + info->mmap_length;
        while ((ptr32_val_t )(mmap) + mmap->size < info_end) {
            if ((mmap->base_addr_low + mmap->length_low) > __mem_size) {
                __mem_size = mmap->base_addr_low + mmap->length_low;
            }

            unsigned long addr = mmap->base_addr_low / 0x1000;
            unsigned long limit = mmap->length_low / 0x1000;

            while (addr < 0x120 && limit > 0) {
                ++addr;
                --limit;
            }

            if (mmap->type == 1) {
                //Skip
            }
            else if (mmap->type == 2 || mmap->type == 3) {
                //Skip
            }
            else {
                break;
            }
            mmap = (MemoryMap *)(((ptr32_val_t)mmap) + mmap->size + sizeof(ptr32_val_t));
        }
    }

    ptr_val_t mem_end_page = (ptr_val_t)__mem_size;
    pageCnt = mem_end_page / PAGE_SIZE;

#if 0
    {
        //ptr_val_t pc = pageCnt;
        ptr_val_t pc = __mem_size;
        unsigned short *tmp =(unsigned short *)(0xB8000+80*2);
        if (pc==0) {
            *tmp = 0x1730;
        }
        while (pc>0) {
            *tmp++ = 0x1730+(pc%10);
            pc/=10;
        }
        //while(1);
    }
#endif
    data = (void*)new Bits(pageCnt);
#if 0
    if (data==NULL) {
        unsigned short *tmp = (unsigned short *)(0xB8050);
        *tmp++ = 0x1745; //E
        *tmp++ = 0x1753; //R
        *tmp++ = 0x1753; //R
        *tmp++ = 0x174d; //M
        while(1);
        return false;
    }
    //BITS(data)->clearAll();
#endif

#if 0
    if (platformData!=NULL) {
        MultibootInfo *info = (MultibootInfo*)platformData;
        MemoryMap *mmap = (MemoryMap*)(info->mmap_addr);
        ptr32_val_t  info_end = info->mmap_addr + info->mmap_length;
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
                    //BITS(data)->set(addr/PAGE_SIZE);
                    //BITS(data)->set(addr);
                    addr++;
                    limit--;
                }
            }
            else if (mmap->type == 2 || mmap->type == 3) {
                //Skip
            }
            else {
                break;
            }
            mmap = (MemoryMap *)(((ptr32_val_t)mmap) + mmap->size + sizeof(ptr32_val_t));
        }
    }
#endif

    directory = (void*)new PageDir();
    while (directory == NULL) ;

#if 1
    //for (uint32_t i=HEAP_START; i<HEAP_END; i+=PAGE_SIZE) {
    for (uint32_t i=HEAP_START; i<HEAP_START + KERNEL_INIT_SIZE; i += PAGE_SIZE) {
        PDIR(directory)->getPage(i, PageDir::PageDoReserve);
    }

    //for (uint32_t i=USER_HEAP_START; i<USER_HEAP_END; i+=PAGE_SIZE) {
    for (uint32_t i = USER_HEAP_START; i < USER_HEAP_START+KERNEL_INIT_SIZE; i += PAGE_SIZE) {
        PDIR(directory)->getPage(i, PageDir::PageDoReserve);
    }
#endif

#if 0
    {
            //ptr_val_t pc = pageCnt;
            //ptr_val_t pc = (ptr_val_t)__free_page_address;
            ptr_val_t pc = (ptr_val_t)get_esp();
            unsigned short *tmp =(unsigned short *)(0xB8000+80*2);
            if (pc==0) {
                    *tmp = 0x1730;
            }
            while (pc>0) {
                    *tmp++ = 0x1730+(pc%10);
                    pc/=10;
            }
            while(1);
    }
#endif
    // Identify mapping
    uint32_t i = 0;
    while (i<(ptr_val_t)0x10000) {
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
        i += PAGE_SIZE;
    }
    i = 0xA0000;
    while (i<(ptr_val_t)0xBFFFF) { //VGA display memory
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
        i += PAGE_SIZE;
    }
    i = 0xC0000;
    while (i<(ptr_val_t)0xC7FFF) { //Video BIOS
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
        i += PAGE_SIZE;
    }
    i = 0xC8000;
    while (i<(ptr_val_t)0xEFFFF) { //Mapped HW
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
        i += PAGE_SIZE;
    }
    i = 0xF0000;
    while (i<(ptr_val_t)0xFFFFF) { //BIOS
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
        i += PAGE_SIZE;
    }
    i = 0x100000;
    while (i<(ptr_val_t)__free_page_address) {
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
        i += PAGE_SIZE;
    }
#if 1
    i = (ptr_val_t)__free_page_address;
    ptr_val_t start = i;
    while (i<(ptr_val_t)start+KERNEL_INIT_SIZE) {
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
        i += PAGE_SIZE;
    }
#endif

#if 0
    for (uint32_t i=HEAP_START; i<HEAP_START+KERNEL_INIT_SIZE; i+=PAGE_SIZE) {
            //mapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), MapPageUser, MapPageReadOnly);
            mapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), MapPageKernel, MapPageRW);
    }

    for (uint32_t i=USER_HEAP_START; i<USER_HEAP_START+KERNEL_INIT_SIZE; i+=PAGE_SIZE) {
            mapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), MapPageUser, MapPageRW);
    }
#endif

    pagingDirectoryChange(PDIR(directory)->getPhys());

    pagingEnable();

    is_ok = true;
    return true;
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
    if (p==NULL) return false;
    bool res = true;

    uint32_t i = addr/PAGE_SIZE;
    //if (BITS(data)->isSet(i)) return false;
    if (BITS(data)->isSet(i)) res = false;

    BITS(data)->set(i);

    p->setPresent(true);

    if (perms==MapPageRW) {
        p->setRw(true);
    } else {
        p->setRw(false);
    }

    if (type==MapPageKernel) {
        p->setUserspace(false);
    } else {
        p->setUserspace(true);
    }

    p->setAddress(addr);

    return res;
}

bool PagingPrivate::mapFrame(Page *p, MapType type, MapPermissions perms)
{
    if (p==NULL) return false;

    //Already mapped
    if (!p->isAvail()) {
        return true;
    } else {
        bool ok = false;
        uint32_t i = BITS(data)->findUnset(&ok);
        if (!ok) {
            //TODO handle out of pages/memory
#if 0
            unsigned short *tmp = (unsigned short *)(0xB8200);
            static int n = 0;
            *tmp = 0x1741+(n++%20);
            tmp =(unsigned short *)(0xB8300);
            if (i==0) {
                    *tmp = 0x1730;
            }
            while (i>0) {
                    *tmp++ = 0x1730+(i%10);
                    i/=10;
            }
#endif
            while(1) ;
            return false;
        }

        BITS(data)->set(i);

        p->setPresent(true);

        if (perms==MapPageRW) p->setRw(true);
        else p->setRw(false);

        if (type==MapPageKernel) p->setUserspace(false);
        else p->setUserspace(true);

        p->setAddress(i*PAGE_SIZE);
        return true;
    }
    return false;
}

/* Get a free physical page */
void *PagingPrivate::getPage()
{
    //mapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), MapPageUser, MapPageRW);
    bool ok = false;
    uint32_t i = BITS(data)->findUnset(&ok);
    if (!ok) {
        //TODO handle out of pages/memory
        return NULL;
    }

    BITS(data)->set(i);
    return (void*)(i*PAGE_SIZE);
}

/* Free physical page corresponding to given virtuall address */
void PagingPrivate::freePage(void *ptr)
{
#if 1
    ptr_val_t i = (ptr_val_t)ptr;
    i /= PAGE_SIZE;
    BITS(data)->clear(i);
#endif
}

bool PagingPrivate::mapPhys(void *phys, ptr_t virt, unsigned int flags)
{
    ptr_val_t i = 0;
#if 1
    if (flags&PAGING_MAP_USER) {
        while ((ptr_val_t)__user_heap_address%PAGE_SIZE!=0) __user_heap_address++;
        i = (ptr_val_t)__user_heap_address;
        __user_heap_address+=PAGE_SIZE;
    } else {
        while ((ptr_val_t)__heap_address%PAGE_SIZE!=0) __heap_address++;
        i = (ptr_val_t)__heap_address;
        __heap_address+=PAGE_SIZE;
    }
#else
    i = (ptr_val_t)phys;
#endif

    Page *p = PDIR(directory)->getPage(i, PageDir::PageDoReserve);
    if (p == NULL) {
        //while(1);
        return false;
    }
    if (!p->isAvail()) {
        return false;
    }

    bool res = true;
    if (flags&PAGING_MAP_USER) {
        res = identityMapFrame(p, (ptr_val_t)phys, MapPageUser, MapPageRW);
    } else {
        res = identityMapFrame(p, (ptr_val_t)phys, MapPageKernel, MapPageRW);
    }

    if (virt != NULL) {
        *virt = i;
    }
    return res;
}

/* Map physical page to virtual */
bool PagingPrivate::map(ptr_t virt, unsigned int flags, unsigned int cnt)
{
    ptr_val_t i = 0;
    ptr_val_t vptr = 0;
    unsigned int left = cnt;
    if (flags & PAGING_MAP_USER) {
        while (((ptr_val_t)__user_heap_address%PAGE_SIZE) != 0) {
            ++__user_heap_address;
        }
        i = (ptr_val_t)__user_heap_address;
        while (left-- > 0) {
            __user_heap_address += PAGE_SIZE;
        }
    } else {
        while (((ptr_val_t)__heap_address%PAGE_SIZE) != 0) {
            ++__heap_address;
        }
        i = (ptr_val_t)__heap_address;
        while (left-- > 0) {
            __heap_address += PAGE_SIZE;
        }
    }

    vptr = i;
    while (cnt > 0) {
        Page *p = PDIR(directory)->getPage(i, PageDir::PageDoReserve);
        if (p == NULL) {
            return false;
        }
        if (!p->isAvail()) {
            return false;
        }

        if (flags & PAGING_MAP_USER) {
            if (!mapFrame(p, MapPageUser, MapPageRW)) return false;
        } else {
            if (!mapFrame(p, MapPageKernel, MapPageRW)) return false;
        }

        --cnt;
        i += PAGE_SIZE;
    }

    if (virt != NULL) {
        *virt = vptr;
    }

    return true;
}

/* Unmap memory at ptr */
void *PagingPrivate::unmap(void *ptr)
{
    ptr_val_t i = (ptr_val_t)ptr;
    i /= PAGE_SIZE;

    Page *p = PDIR(directory)->getPage(i, PageDir::PageDontReserve);
    if (p==NULL) return NULL;

    if (p->isAvail()) {
        p->setPresent(false);
        p->setRw(false);
        p->setUserspace(false);
        p->setAddress(0);

        BITS(data)->clear(i);
    }
    return NULL;
}

ptr8_t PagingPrivate::freePageAddress()
{
    ptr_val_t i = (ptr_val_t)__free_page_address/PAGE_SIZE;
    if (!BITS(data)->isSet(i)) {
#if 0
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
#else
        BITS(data)->set(i);
#endif
    }
    return __free_page_address;
}

void PagingPrivate::incFreePageAddress(ptr_val_t size)
{
    ptr_val_t i = (ptr_val_t)__free_page_address/PAGE_SIZE;
    if (!BITS(data)->isSet(i)) {
#if 0
        identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
#else
        BITS(data)->set(i);
#endif
    }
    __free_page_address += size;
}

void PagingPrivate::pageAlign(ptr_val_t align)
{
    /* Align to proper boundaries */
#if 1
    while (((ptr32_val_t)__free_page_address & (align-1)) != 0) {
        BITS(data)->set((ptr_val_t)__free_page_address / PAGE_SIZE);
        __free_page_address++;
    }
#else
    ptr8_t tmp = freePageAddress();
    while (((ptr32_val_t)tmp & (align-1))!=0) {
        tmp++;
        incFreePageAddress(1);
    }
#endif
}

ptr_val_t PagingPrivate::memSize()
{
    return __mem_size;
}

PageDir::PageDir()
{
    for (int i = 0; i < TABLES_PER_DIRECTORY; ++i) {
        tables[i] = NULL;
        tablesPhys[i] = 0;
    }
    phys = (ptr_t)tablesPhys;
}

PageTable *PageDir::getTable(uint32_t i)
{
    if (i < TABLES_PER_DIRECTORY) {
        return tables[i];
    }
    return NULL;
}

Page *PageDir::getPage(ptr_val_t addr, PageReserve reserve)
{
    addr /= PAGE_SIZE;
    uint32_t index = addr / PAGES_PER_TABLE;

    //We already have the table
    if (tables[index] != NULL) {
        return tables[index]->get(addr % PAGES_PER_TABLE);
    }
    else if (reserve==PageDoReserve) {
        ptr_val_t physPtr = 0;
        tables[index] = new ((ptr_t)&physPtr) PageTable();
        if (physPtr == 0) {
/*
            unsigned short *vid = (unsigned short *)(0xB8000);
            *vid = 0x1745; //E
            while (1);
*/
            return NULL;
        }
        tablesPhys[index] = physPtr | PAGING_MAP_R2;
        return tables[index]->get(addr % PAGES_PER_TABLE);
    }
    return NULL;
}

void PageDir::copyTo(PageDir *dir)
{
    ptr_t offs = (ptr_t)((ptr_val_t)dir->tablesPhys - (ptr_val_t)dir);
    dir->phys = (ptr_val_t)phys + offs;
    for (int i = 0; i < TABLES_PER_DIRECTORY; i++) {
        if (tables[i] != NULL) {
            if (1) {
                dir->tables[i] = tables[i];
                dir->tablesPhys[i] = tablesPhys[i];
            } else {
                ptr_val_t tmp;
                dir->tables[i] = new (&tmp) PageTable();
                dir->tablesPhys[i] = tmp | PAGING_MAP_R2;
                tables[i]->copyTo(dir->tables[i]);
            }
        }
    }
}

void paging_mmap_init(MultibootInfo *info)
{
    /* Initialize paging */
    Paging p;
    p.init((void*)info);
}
