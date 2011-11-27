#include "pagingx86.h"
#include "types.h"
#include "x86.h"
#include "bits.h"

#define PAGING_START_POS 0x40000000
//#define PAGE_SIZE PAGING_SIZE
#define PDIR(x) ((PageDir*)x)
#define BITS(x) ((Bits*)x)

#define HEAP_START         0xC0000000
#define HEAP_INITIAL_SIZE  0x100000
#define HEAP_END           0xCFFFFFFF
#define USER_HEAP_START    0xD0000000
#define USER_HEAP_INITIAL_SIZE 0x100000
#define USER_HEAP_END      0xDFFFFFFF

static ptr32_t __page_directory    = (ptr32_t)0xFFFFF000; //TODO
static ptr32_t __page_table        = (ptr32_t)0xFFC00000;
static ptr8_t  __free_page_address = (ptr8_t)PAGING_START_POS;
static ptr8_t      __mem_map       = (ptr8_t)(KERNEL_VIRTUAL+0x500);
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
ptr32_val_t __page_mapping_mutex       = 0;
ptr32_val_t __page_mapping_static_mutex       = 0;

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
	if (page==NULL) return;

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
	static unsigned short a = 0;
	unsigned short *tmp = (unsigned short *)(0xB8002);
	*tmp = 0x1741+a;
	(++a)%=22;
}

Page *PageTable::get(uint32_t i)
{
	if (i<PAGES_PER_TABLE) return &pages[i];
	return NULL;
}

bool PageTable::copyTo(PageTable *table)
{
	if (table==NULL) return false;

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

extern uint32_t kernel_end;
PagingPrivate::PagingPrivate()
{
	m.assign(&__page_mapping_mutex);
	m_static.assign(&__page_mapping_static_mutex);
	data = NULL;
	pageCnt = 0;
	is_ok = false;

	// Map free space after kernel
	__free_page_address = (ptr8_t)kernel_end;
}

PagingPrivate::~PagingPrivate()
{
}

bool PagingPrivate::init(void *platformData)
{
	if (data!=NULL) return false;
	is_ok = false;

	cli();
	pagingDisable(); //Safety

	(void)platformData;
	unsigned short *tmp = (unsigned short *)(0xB8000);
	*tmp = 0x1744; //D
	data = (void*)new Bits(pageCnt);
	BITS(data)->clearAll();
	*tmp = 0x1745; //E

	if (platformData!=NULL) {
		MultibootInfo *info = (MultibootInfo*)platformData;
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
					//__mem_map[addr/8] |= 1 << addr % 8;
					BITS(data)->set(addr/8);
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
	*tmp = 0x1746; //F
#if 0
	/* handle data */
	if (platformData!=NULL) {
		MultibootInfo *info = (MultibootInfo*)platformData;
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
					//BITS(data)->set(addr/8);
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

	*tmp = 0x1747; //G
	ptr_val_t mem_end_page = (ptr_val_t)__mem_size;
	pageCnt = mem_end_page/PAGE_SIZE;
	*tmp = 0x1748; //H
	BITS(data)->copyFrom((uint32_t*)__mem_map);
	directory = (void*)new PageDir();
	*tmp = 0x1749; //I

#if 1
	//for (uint32_t i=HEAP_START; i<HEAP_END; i+=PAGE_SIZE) {
	for (uint32_t i=HEAP_START; i<HEAP_START+HEAP_INITIAL_SIZE; i+=PAGE_SIZE) {
		PDIR(directory)->getPage(i, PageDir::PageDoReserve);
	}
	*tmp = 0x2149; //I

	//for (uint32_t i=USER_HEAP_START; i<USER_HEAP_END; i+=PAGE_SIZE) {
	for (uint32_t i=USER_HEAP_START; i<USER_HEAP_START+USER_HEAP_INITIAL_SIZE; i+=PAGE_SIZE) {
		PDIR(directory)->getPage(i, PageDir::PageDoReserve);
	}
#endif

	*tmp = 0x174a; //J
	// Identify mapping
	uint32_t i = 0;
	while (i<(ptr_val_t)__free_page_address) {
		mapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), MapPageKernel, MapPageRW);
		i += PAGE_SIZE;
	}
	*tmp = 0x274a; //J

	for (uint32_t i=HEAP_START; i<HEAP_START+HEAP_INITIAL_SIZE; i+=PAGE_SIZE) {
		mapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), MapPageUser, MapPageReadOnly);
	}
	*tmp = 0x374a; //J

	for (uint32_t i=USER_HEAP_START; i<USER_HEAP_START+USER_HEAP_INITIAL_SIZE; i+=PAGE_SIZE) {
		mapFrame(PDIR(directory)->getPage(i, PageDir::PageDoReserve), MapPageUser, MapPageRW);
	}
	*tmp = 0x174b; //K

	pagingDirectoryChange(PDIR(directory)->getPhys());
	*tmp = 0x174c; //L
	pagingEnable();

	*tmp = 0x174d; //M
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

void PagingPrivate::mapFrame(Page *p, MapType type, MapPermissions perms)
{
	if (p==NULL) return;

	//Already mapped
	if (!p->isAvail()) {
		return;
	} else {
		bool ok = false;
		uint32_t i = BITS(data)->findUnset(&ok);
		if (!ok) {
			//TODO handle out of pages/memory
			return;
		}

		BITS(data)->set(i);
		p->setPresent(true);

		if (perms==MapPageRW) p->setRw(true);
		else p->setRw(false);

		if (type==MapPageKernel) p->setUserspace(false);
		else p->setUserspace(true);

		p->setAddress(i*PAGE_SIZE);
	}
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
#if 0
	bool ok;
	BITS(data)->findUnused(&ok);
	uint32_t i 
#endif
#if 0
	static unsigned int count = 0;
	unsigned char t = 0;

	while (1) {
		// Found a free page?
		if ((__mem_map[count] & (1<<t)) != 0) {
			unsigned char i = 0;

			// Mark it used
			__mem_map[count] ^= (1<<t);
			while (t>0) {
				i++;
				t--;
			}
			//return (void *)(((count*8)+i)*PAGE_SIZE);
			return (void *)(((count<<3)+i)*PAGE_SIZE);
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
#endif
}

/* Free physical page corresponding to given virtuall address */
void PagingPrivate::freePage(void *ptr)
{
	ptr_val_t i = (ptr_val_t)ptr;
	i/=PAGE_SIZE;
	BITS(data)->clear(i);
#if 0
	ptr32_val_t count = (((ptr32_val_t)ptr) / 0x8000);
        ptr8_val_t  bit   = ((((ptr32_val_t)ptr) / 0x1000) % 8);

	__mem_map[count] |= bit;
#endif
}

void *Paging::getPage()
{
	_d->lock();
	void *p = _d->getPage();
	_d->unlock();
	return p;
}

void Paging::map(void *phys, void *virt, unsigned int flags)
{
	_d->lock();

#if 0
	ptr32_val_t pagedir   = (ptr32_val_t)virt >> 22;
	ptr32_val_t pagetable = (ptr32_val_t)virt >> 12 & 0x3FF;
	bool do_map = false;

        if ((pd[pagedir] & 1) != 1)  do_map = true;
        ptr32_t pt = __page_table + (0x400 * pagedir);
        if ((pt[pagetable] & 1) == 1) do_map = true;

	if (do_map) _d->map(phys,virt,flags);
#endif
	_d->map(phys,virt,flags);
	_d->unlock();
}

/* Map physical page to virtual */
bool PagingPrivate::map(void *phys, void *virt, unsigned int flags)
{
	ptr32_val_t pagedir   = (ptr32_val_t)virt >> 22;
	ptr32_val_t pagetable = (ptr32_val_t)virt >> 12 & 0x3FF;

	//unsigned long *pd = (unsigned long *) 0xFFFFF000;
	//unsigned short *vid = (unsigned short *)(KERNEL_VIRTUAL+0xB8000);
	//*vid = 0x4741; //A
	ptr32_t pd = __page_directory;
	//VideoX86 v;
	//v.printf("\n\n\n\n\n\nMAP: %x [%x]\n",pd,pagedir);
        if ((pd[pagedir] & 1) != 1) {
		void *page = getPage();
                pd[pagedir] = ((ptr32_val_t)page) | PAGING_MAP_R0;
        }

        //ptr32_t pt = (ptr32_t )0xFFC00000 + (0x400 * pagedir);
        //ptr32_t pt = (ptr32_t)((ptr32_val_t)__page_table + (0x400 * pagedir));
        ptr32_t pt = __page_table + (0x400 * pagedir);
        if ((pt[pagetable] & 1) == 1) {
		unsigned short *vid = (unsigned short *)(KERNEL_VIRTUAL+0xB8000);
		*vid = 0x4745; //E
		*(++vid) = 0x4752; //R
		*(++vid) = 0x4752; //R
		cli();
		hlt();
		//Platform::seizeInterrupts();
		//Platform::halt();
		return false;
        }

        pt[pagetable] = ((ptr32_val_t)phys) | (flags & 0xFFF) | 0x01;

	return true;
}

/* Unmap memory at ptr */
void *PagingPrivate::unmap(void *ptr)
{
	ptr32_val_t pagedir   = (ptr32_val_t)ptr >> 22;
	ptr32_val_t pagetable = (ptr32_val_t)ptr >> 12 & 0x3FF;
	
        ptr32_t pt = __page_table + (0x400 * pagedir);
        if ((pt[pagetable] & 1) == 0) {
		unsigned short *vid = (unsigned short *)(KERNEL_VIRTUAL+0xB8000);
		*vid = 0x1745; //E
		cli();
		hlt();
		//Platform::seizeInterrupts();
		//Platform::halt();
		return NULL;
	}

	pt[pagetable] &= 0xFFFFFFFE;
	return (void *) (pt[pagetable] & 0xFFFFF000);
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
	/* Align to proper boundaries */
#if 1
	//ptr8_t tmp = freePageAddress();
	while (((ptr32_val_t)__free_page_address & (align-1))!=0) {
		//tmp++;
		__free_page_address++;
		//incFreePageAddress(1);
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
	for (int i=0; i<TABLES_PER_DIRECTORY; i++) {
		tables[i] = NULL;
		tablesPhys[i] = 0;
	}
	phys = (ptr_t)tablesPhys;
}

PageTable *PageDir::getTable(uint32_t i)
{
	if (i<TABLES_PER_DIRECTORY) {
		return tables[i];
	}
	return NULL;
}

Page *PageDir::getPage(ptr_val_t addr, PageReserve reserve)
{
	uint32_t index;
	addr /= PAGE_SIZE;
	index = addr / PAGES_PER_TABLE;

	//We already have the table
	if (tables[index]!=NULL) {
		return tables[index]->get(addr%PAGES_PER_TABLE);
	}
	else if (reserve==PageDoReserve) {
		ptr_val_t physPtr = 0 ;
		//tables[index] = (PageTable*)reserveStatic(sizeof(PageTable),&physPtr);
		//new(&physPtr) PageTable();
		tables[index] = new ((ptr_t)&physPtr) PageTable();
		//tables[index] = new PageTable();
		physPtr = (ptr_val_t)tables[index];
		tablesPhys[index] = physPtr | PAGING_MAP_R2;
		return tables[index]->get(addr%PAGES_PER_TABLE);
	}
	return NULL;
}

void PageDir::copyTo(PageDir *dir)
{
	ptr_t offs = (ptr_t)((ptr_val_t)dir->tablesPhys - (ptr_val_t)dir);
	dir->phys = (ptr_val_t)phys + offs;
	for (int i=0; i<TABLES_PER_DIRECTORY; i++) {
		if (tables[i]!=NULL) {
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

#if 0
void PagingPrivate::setFreePageAddress(ptr8_t pos)
{
	__free_page_address=pos;
}
#endif

void paging_mmap_init(MultibootInfo *info)
{
#if 0
	MemoryMap *mmap = (MemoryMap*)(info->mmap_addr);
	ptr32_val_t  info_end = info->mmap_addr + info->mmap_length;

	unsigned short *tmp = (unsigned short *)(0xB8000);
	*tmp = 0x1744; //D
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
			break;
		}
		mmap = (MemoryMap *)(((ptr32_val_t)mmap) + mmap->size + sizeof(ptr32_val_t));
	}
	*tmp = 0x1745; //E
#endif
#if 0
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
			break;
		}
		mmap = (MemoryMap *)(((ptr32_val_t)mmap) + mmap->size + sizeof(ptr32_val_t));
	}
#endif
	/* Initialize paging */
	Paging p;
	p.init((void*)info);
#if 0
	/* Clear the mappings */
	ptr32_val_t PD_MAX = __mem_size/PAGE_SIZE+1;
	//PD_MAX = 1024;
	ptr32_t pd = __page_directory;
	for (ptr32_val_t n=0; n<PD_MAX; n++) {
		pd[n] = 0;
        	ptr32_t pt = __page_table + (0x400 * n);
		//for (ptr32_val_t m=0; m<PD_MAX; m++) {
		for (ptr32_val_t m=0; m<0x400; m++) {
			pt[m] = 0;
		}
	}
#endif
}

