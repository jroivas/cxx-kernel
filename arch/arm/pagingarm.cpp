#include "pagingarm.h"
#include "types.h"
//#include "x86.h"
#include "bits.h"

#define PDIR(x) ((PageDir*)x)
#define BITS(x) ((Bits*)x)

//#define KERNEL_PRE_POS     0x20000000
#define KERNEL_PRE_POS     0x200000 //FIXME
#define KERNEL_INIT_SIZE   0x000FFFFF
#define HEAP_START         0x30000000
#define HEAP_END           0x3FFFFFFF
#define USER_HEAP_START    0x40000000
#define USER_HEAP_END      0x4FFFFFFF

static ptr8_t  __free_page_address = (ptr8_t)KERNEL_PRE_POS;
//static ptr8_t  __heap_address      = (ptr8_t)HEAP_START;
//static ptr8_t  __user_heap_address = (ptr8_t)USER_HEAP_START;
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

#if 0
extern "C" void pagingEnable();
extern "C" void pagingDisable();
extern "C" void pagingDirectoryChange(ptr_t a);
extern "C" void copyPhysicalPage(ptr_val_t a, ptr_val_t b);
#endif
extern unsigned int kernel_end;

extern "C" void pagingEnable()
{
    asm("mrc p15, 0, r0, c1, c0, 0");
    asm("orr r0, r0, #0x1");
    asm("mrc p15, 0, r0, c1, c0, 0");
}

extern "C" void pagingDisable()
{
    // Do we really want to do this?
    asm("mrc p15, 0, r0, c1, c0, 0");
    //asm("and r0, r0, #0xfffffffe");
    asm("bic r0, r0, #0x1");
    asm("mrc p15, 0, r0, c1, c0, 0");
}

extern "C" void setPagingDirectory(unsigned int table)
{
    asm("ldr r0, %[table]" : : [table] "imm" (table) );
    asm("mcr p15, 0, r0, c2, c0, 0");
}

//Mutex for locking mapping and prevent theading/SMP problems
ptr32_val_t __page_mapping_mutex          = 0;
ptr32_val_t __page_mapping_static_mutex   = 0;

void Page::copyPhys(Page p)
{
    (void)p;
    //copyPhysicalPage(getAddress(), p.getAddress());
}

void Page::alloc(PageType type)
{
    if (!isAvail()) return;
    Paging p;
    void *page = p.getPage();
    if (page==nullptr) return;

    setAddress((ptr_val_t)page);

    setPresent(true);
    setRw(true);
    if (type==KernelPage) {
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
    if (i<PAGES_PER_TABLE) return &pages[i];
    return nullptr;
}

bool PageTable::copyTo(PageTable *table)
{
    if (table==nullptr) return false;

    //Paging p;
    for (int i=0; i<PAGES_PER_TABLE; i++) {
            if (pages[i].isAvail()) {
                    table->pages[i].alloc();
                    pages[i].copyTo(&table->pages[i]);
                    pages[i].copyPhys(table->pages[i]);
            }
    }

    return true;
}

ptr32_val_t roundTo4k(ptr32_val_t s)
{
    while (s%4096!=0) s++;
    return s;
}

ptr32_val_t roundTo1k(ptr32_val_t s)
{
    while (s%1024!=0) s++;
    return s;
}


#if 0
ptr32_val_t firstLevelTableSection(ptr32_val_t section, ptr32_val_t domain, PageDir::MMUPermissions permissions)
{
    //0x000-0xFFF
    ptr32_val_t val = 0;
    val |= (section & 0xFFF) << 20;
    val |= 2; //Section desc

    switch(permissions) {
        case MMU_NO_NO:
            // Should set System=0 and Rom=0 in CP15
            break;
        case MMU_RO_NO:
            // Should set System=1 and Rom=0 in CP15
            break;
        case MMU_RO_RO:
            // Should set System=0 and Rom=1 in CP15
            break;
        case MMU_RW_NO:
            //01
            val |= (1<<10);
            break;
        case MMU_RW_RO:
            //10
            val |= (2<<10);
            break;
        case MMU_RW_RW:
            //11
            val |= (3<<10);
            break;
    };

    val |= (domain & 0x1f) << 5;

    return val;
}

ptr32_val_t firstLevelTableCoarse(ptr32_val_t addr, ptr32_val_t domain)
{
    ptr32_val_t val = 0;
    val |= (addr & 0xFFFFF800);
    val |= 1; //Coarse page desc

    val |= (domain & 0x1f) << 5;

    return val;
}

ptr32_val_t secondLevelTable(ptr32_val_t addr, ptr32_val_t level, MMUPermissions permissions)
{
    ptr32_val_t val = 0;
    if (level==0x1) {
        val |= (addr & 0xFFFF0000);
    }
    else if (level==0x2) {
        val |= (addr & 0xFFFFF000);
    }
    else if (level==0x3) {
        val |= (addr & 0xFFFFFC00);
    }
    else {
        //ERROR!!! Handle?
        return 0;
    }
    val |= level;

    ptr32_val_t per = 0;
    switch(permissions) {
        case MMU_NO_NO:
            // Should set System=0 and Rom=0 in CP15
            break;
        case MMU_RO_NO:
            // Should set System=1 and Rom=0 in CP15
            break;
        case MMU_RO_RO:
            // Should set System=0 and Rom=1 in CP15
            break;
        case MMU_RW_NO:
            //01
            per = 1;
            break;
        case MMU_RW_RO:
            //10
            per = 2;
            break;
        case MMU_RW_RW:
            //11
            per = 3;
            break;
    };
    val |= (per<<4);
    if (level!=0x3) {
        val |= (per<<6);
        val |= (per<<8);
        val |= (per<<10);
    }

    return val;
}
#endif

PageDir::PageDir(uint32_t physLoc)
{
    phys = (ptr_t)physLoc;
    uint32_t tmp = physLoc;
    for (uint32_t i = 0; i<0x1000; i++) {
        *((uint32_t*)tmp) = 0;
        tmp += 4;
    }
    free_pos = 0;
    free_page_section = 1;
    free_page_pos = 0;
}

uint32_t PageDir::getPage()
{
    if (free_page_pos<1024) {
        uint32_t p = free_page_pos;
        free_page_pos += 16;
        uint32_t table = getCoarseTable(free_pos);
        (void)p;
        (void)table;
    }
    return 0;
}

uint32_t PageDir::permissions(PageDir::MMUPermissions permissions)
{
    uint32_t tmp = 0;
    switch(permissions) {
        case MMU_NO_NO:
            // Should set System=0 and Rom=0 in CP15
            break;
        case MMU_RO_NO:
            // Should set System=1 and Rom=0 in CP15
            break;
        case MMU_RO_RO:
            // Should set System=0 and Rom=1 in CP15
            break;
        case MMU_RW_NO:
            //01
            tmp = 1;
            break;
        case MMU_RW_RO:
            //10
            tmp = 2;
            break;
        case MMU_RW_RW:
            //11
            tmp = 3;
            break;
    };

    return tmp;
}

ptr32_val_t PageDir::createSection(ptr32_val_t section, ptr32_val_t addr, ptr32_val_t domain, PageDir::MMUPermissions section_permissions)
{
    ptr32_val_t val = 0;

    //uint32_t section = (addr >> 20) & 0xFFF;
    section &= 0xFFF;

    val |= (addr & 0xFFF00000);
    val |= 2; //Section desc

    val |= (permissions(section_permissions)<<10);
    val |= (domain & 0x1f) << 5;

    ptr_t pos = phys;
    pos[section] = val;

    return val;
}

ptr32_val_t PageDir::setCoarseTable(ptr32_val_t section, ptr32_val_t val)
{
    section &= 0xFFF;

    ptr_t pos = phys;
    pos[section] = val;

    return val;
}

ptr32_val_t PageDir::getCoarseTable(ptr32_val_t section)
{
    ptr_t pos = phys;
    ptr32_val_t val = pos[section];
    return (val & 0xFFF00000);
}

//Just returns te value, need to set in table
ptr32_val_t PageDir::createCoarseTable(ptr32_val_t addr, ptr32_val_t domain, PageDir::MMUPermissions section_permissions)
{
    ptr32_val_t val = 0;

    val |= (addr & 0xFFFFF800);
    val |= 1; //Coarse page desc

    val |= (domain & 0x1f) << 5;

    ptr32_val_t per = permissions(section_permissions);
    val |= (per<<4);
    val |= (per<<6);
    val |= (per<<8);
    val |= (per<<10);

    return val;
}

//B3-13

/*
extern uint32_t kernel_end;
extern uint32_t my_kernel_end;
*/
PagingPrivate::PagingPrivate()
{
    m.assign(&__page_mapping_mutex);
    m_static.assign(&__page_mapping_static_mutex);
    data = nullptr;
    pageCnt = 0;
    __mem_size = 0;
    is_ok = false;

    // Map free space after kernel
    //__free_page_address = (ptr8_t)my_kernel_end;

    // Create the table, we're spending some memory here...
    if (kernel_end>(ptr_val_t)__free_page_address)
        __free_page_address = (ptr8_t)kernel_end;
    __free_page_address = (ptr8_t)roundTo4k((ptr32_val_t)__free_page_address);
    ptr32_t pagingDir = (ptr32_t)__free_page_address;

    PageDir *pdir = new PageDir((ptr32_val_t)pagingDir);
    pdir->incFreePos();

    // Get the free addr

    // Ok, setting the first level paging dir
    //FIXME:
    setPagingDirectory((ptr32_val_t)pagingDir);
    ptr32_t pd = pagingDir;
    ptr32_val_t tmp = pdir->createSection(0, 0, 0, PageDir::MMU_RW_RW);
    *pd++ = tmp;
#if 1
    tmp = pdir->createSection(1, 0, 0, PageDir::MMU_RW_RW);
    *pd++ = tmp;
    tmp = pdir->createSection(2, 0, 0, PageDir::MMU_RW_RW);
    *pd++ = tmp;
#endif

    //Reserve it
    __free_page_address += 4096*4;


    // Round it
    __free_page_address = (ptr8_t)roundTo1k((ptr32_val_t)__free_page_address);
    ptr_val_t first_coarse = (ptr_val_t)__free_page_address;
    __free_page_address += 1024;
    ptr_val_t cc = pdir->createCoarseTable(first_coarse, 0, PageDir::MMU_RW_RW);
    pdir->incFreePos();
    pdir->setCoarseTable(pdir->getFreePos(), cc);

    directory = (void*)pdir;
}

PagingPrivate::~PagingPrivate()
{
}

bool PagingPrivate::init(void *platformData)
{
    (void)platformData;
    if (data!=nullptr) return false;
    is_ok = false;

#if 0
    pagingDisable(); //Safety
#endif


#if 0
    if (platformData!=nullptr) {
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
    pageCnt = mem_end_page/PAGE_SIZE;

    data = (void*)new Bits(pageCnt);

    directory = (void*)new PageDir();
    while (directory==nullptr) ;
#endif

#if 0
    //for (uint32_t i=HEAP_START; i<HEAP_END; i+=PAGE_SIZE) {
    for (uint32_t i=HEAP_START; i<HEAP_START+KERNEL_INIT_SIZE; i+=PAGE_SIZE) {
            PDIR(directory)->getPage(i, PageDir::PageDoReserve);
    }

    //for (uint32_t i=USER_HEAP_START; i<USER_HEAP_END; i+=PAGE_SIZE) {
    for (uint32_t i=USER_HEAP_START; i<USER_HEAP_START+KERNEL_INIT_SIZE; i+=PAGE_SIZE) {
            PDIR(directory)->getPage(i, PageDir::PageDoReserve);
    }
#endif

#if 0
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
#endif
#if 0
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

    pagingDirectoryChange(PDIR(directory)->getPhys());
#endif

#if 0
    pagingEnable();

    is_ok = true;
#endif
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
    if (p==nullptr) return false;
    bool res = true;

    uint32_t i = addr/PAGE_SIZE;
    //if (BITS(data)->isSet(i)) return false;
    if (BITS(data)->isSet(i)) res = false;
    
    BITS(data)->set(i);

    p->setPresent(true);

    if (perms==MapPageRW) p->setRw(true);
    else p->setRw(false);

    if (type==MapPageKernel) p->setUserspace(false);
    else p->setUserspace(true);

    p->setAddress(addr);

    return res;
}

bool PagingPrivate::mapFrame(Page *p, MapType type, MapPermissions perms)
{
    if (p==nullptr) return false;

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
        return nullptr;
    }

    BITS(data)->set(i);
    return (void*)(i*PAGE_SIZE);
}

/* Free physical page corresponding to given virtuall address */
void PagingPrivate::freePage(void *ptr)
{
#if 1
    ptr_val_t i = (ptr_val_t)ptr;
    i/=PAGE_SIZE;
    BITS(data)->clear(i);
#endif
}

bool PagingPrivate::mapPhys(void *phys, ptr_t virt, unsigned int flags)
{
    (void)phys;
    (void)virt;
    (void)flags;
#if 0
#if 1
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
    if (p==nullptr) {
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
    
    if (virt!=nullptr) {
        *virt = i;
    }
    return res;
#endif
#endif
    return false;
}

/* Map physical page to virtual */
bool PagingPrivate::map(ptr_t virt, unsigned int flags, unsigned int cnt)
{
    (void)flags;
#if 0
    ptr_val_t i = 0;
    ptr_val_t vptr = 0;
    unsigned int left = cnt;
    if (flags&PAGING_MAP_USER) {
            while (((ptr_val_t)__user_heap_address%PAGE_SIZE)!=0) __user_heap_address++;
            i = (ptr_val_t)__user_heap_address;
            while (left-->0) 
                    __user_heap_address+=PAGE_SIZE;
    } else {
            while (((ptr_val_t)__heap_address%PAGE_SIZE)!=0) __heap_address++;
            i = (ptr_val_t)__heap_address;
            while (left-->0) 
                    __heap_address+=PAGE_SIZE;
    }

    vptr = i;
    while (cnt>0) {
            Page *p = PDIR(directory)->getPage(i, PageDir::PageDoReserve);
            if (p==nullptr) {
                    return false;
            }
            if (!p->isAvail()) {
                    return false;
            }

            if (flags&PAGING_MAP_USER) {
                    if (!mapFrame(p, MapPageUser, MapPageRW)) return false;
            } else {
                    if (!mapFrame(p, MapPageKernel, MapPageRW)) return false;
            }

            cnt--;
            i+=PAGE_SIZE;
    }

    if (virt!=nullptr) {
            *virt = vptr;
    }
#endif
    ptr_val_t vptr = (ptr_val_t)roundTo1k((ptr32_val_t)__free_page_address);
    while (cnt>0) {
        __free_page_address += PAGE_SIZE;
        cnt--;
    }
            
    if (virt!=nullptr) {
        *virt = vptr;
    }

    return true;
}

/* Unmap memory at ptr */
void *PagingPrivate::unmap(void *ptr)
{
    (void)ptr;
#if 0
    ptr_val_t i = (ptr_val_t)ptr;
    i/=PAGE_SIZE;

    Page *p = PDIR(directory)->getPage(i, PageDir::PageDontReserve);
    if (p==nullptr) return nullptr;

    if (p->isAvail()) {
            p->setPresent(false);
            p->setRw(false);
            p->setUserspace(false);
            p->setAddress(0);

            BITS(data)->clear(i);
    }
#endif
    return nullptr;
}

ptr8_t PagingPrivate::freePageAddress()
{
#if 0
    ptr_val_t i = (ptr_val_t)__free_page_address/PAGE_SIZE;
    if (!BITS(data)->isSet(i)) {
#if 0
            identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
#else
            BITS(data)->set(i);
#endif
    }
#endif
    return __free_page_address;
}

void PagingPrivate::incFreePageAddress(ptr_val_t size)
{
#if 0
    ptr_val_t i = (ptr_val_t)__free_page_address/PAGE_SIZE;
    if (!BITS(data)->isSet(i)) {
#if 0
            identityMapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), i, MapPageKernel, MapPageRW);
#else
            BITS(data)->set(i);
#endif
    }
#endif
    __free_page_address += size;
}

void PagingPrivate::pageAlign(ptr_val_t align)
{
    /* Align to proper boundaries */
#if 1
#if 0
    while (((ptr32_val_t)__free_page_address & (align-1))!=0) {
            BITS(data)->set((ptr_val_t)__free_page_address/PAGE_SIZE);
            __free_page_address++;
    }
#else
    ptr8_t tmp = freePageAddress();
    while (((ptr32_val_t)tmp & (align-1))!=0) {
            tmp++;
            incFreePageAddress(1);
    }
#endif
#endif
}

ptr_val_t PagingPrivate::memSize()
{
    return __mem_size;
}

#if 0
PageDir::PageDir()
{
    for (int i=0; i<TABLES_PER_DIRECTORY; i++) {
            tables[i] = nullptr;
            tablesPhys[i] = 0;
    }
    phys = (ptr_t)tablesPhys;
}

PageTable *PageDir::getTable(uint32_t i)
{
    if (i<TABLES_PER_DIRECTORY) {
            return tables[i];
    }
    return nullptr;
}

Page *PageDir::getPage(ptr_val_t addr, PageReserve reserve)
{
    addr/=PAGE_SIZE;
    uint32_t index = addr / PAGES_PER_TABLE;

    //We already have the table
    if (tables[index]!=nullptr) {
            return tables[index]->get(addr%PAGES_PER_TABLE);
    }
    else if (reserve==PageDoReserve) {
            ptr_val_t physPtr = 0;
            tables[index] = new ((ptr_t)&physPtr) PageTable();
            if (physPtr==0) {
/*
                    unsigned short *vid = (unsigned short *)(0xB8000);
                    *vid = 0x1745; //E
                    while (1);
*/
                    return nullptr;
            }
            tablesPhys[index] = physPtr | PAGING_MAP_R2;
            return tables[index]->get(addr%PAGES_PER_TABLE);
    }
    return nullptr;
}

void PageDir::copyTo(PageDir *dir)
{
    ptr_t offs = (ptr_t)((ptr_val_t)dir->tablesPhys - (ptr_val_t)dir);
    dir->phys = (ptr_val_t)phys + offs;
    for (int i=0; i<TABLES_PER_DIRECTORY; i++) {
            if (tables[i]!=nullptr) {
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
#endif

static Paging p;
void paging_mmap_init(MultibootInfo *info)
{
    /* Initialize paging */
    p.init((void*)info);
}

