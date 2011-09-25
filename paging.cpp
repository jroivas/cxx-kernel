#include "paging.h"
#include "types.h"
#include "mutex.h"

#define PAGE_CNT 1024
//#define VIRT_TO_PHYS 0x40000000
#define LPOS 0x100000
//#define PAGING_POS 0x10000
#define PAGING_START_POS 0x1000
//#define PAGE_SIZE 4096

#define PAGE_MAP_CNT 1
#define PAGE_MAP_MUTEX 2
#define PAGE_MAP_INDEX 4

//#define PAGING_SIZE 0x1000
#define PAGING_SIZE (sizeof(unsigned int*)*PAGE_CNT) // A little bit more portable
#define PAGE_SIZE PAGING_SIZE

unsigned int *__page_directory = (unsigned int*) 0x10000; //TODO FIXME
int __paging_cnt = 0;
unsigned int *__free_page_address = (unsigned int*)PAGING_START_POS;

struct MemoryMap
{
        unsigned long size;
        unsigned long base_addr_low;
        unsigned long base_addr_high;
        unsigned long length_low;
        unsigned long length_high;
        unsigned long type;
};

//Mutex for locking mapping and prevent theading/SMP problems
static unsigned int __page_mapping_mutex = 0;

class PageMap
{
public:
	PageMap(unsigned int *addr);
	void init();
	bool canAllocPage();
	unsigned char *map(unsigned int cnt);
	//void forceMap(unsigned int i);
	void freePages(unsigned int *ptr, unsigned int cnt);

private:
	class MetaPage
	{
	public:
		MetaPage();
		MetaPage(unsigned int *struct_addr);
		void initialize(unsigned int *addr);
		unsigned char *map(unsigned int cnt, unsigned int *freeaddr);

		unsigned int *address(unsigned int index=0);
		unsigned int count();
		void increaseCnt();
		bool isFree(unsigned int index);
		void setFree(unsigned int index);
		void setUsed(unsigned int index);
		bool isAvailablePages(unsigned int cnt=1);
		unsigned int findFreePages(unsigned int cnt=1);
		void lock();
		void unlock();
		void mapPage(unsigned int index, unsigned int *addr);

	private:
		//unsigned int *m_addr_page;
		//unsigned int *m_addr;
		unsigned int *m_addr;
		unsigned int *m_index;
		Mutex m;
	};

	unsigned int *m_addr;
	MetaPage meta;
	Mutex m;
};

PageMap::PageMap(unsigned int *addr)
{
	m_addr = addr;
	meta = MetaPage(m_addr);
	m = Mutex(&__page_mapping_mutex);
}

#if 0
void PageMap::forceMap(unsigned int i)
{
	m.lock();
	meta.lock();
	meta.setUsed(i);
	meta.unlock();
	m.unlock();
}
#endif

bool PageMap::canAllocPage()
{
	m.lock();
	if (meta.isAvailablePages(1)) {
		m.unlock();
		return true;
	}
	if (meta.findFreePages(1)>0) {
		m.unlock();
		return true;
	}

	m.unlock();
	return false;
}

unsigned char *PageMap::map(unsigned int cnt)
{
	m.lock();
	unsigned char *tmp = meta.map(cnt, __free_page_address);
	if (tmp!=NULL) {
		__free_page_address += (PAGE_SIZE*cnt);
	}
	m.unlock();
	return tmp;
}

void PageMap::freePages(unsigned int *ptr, unsigned int cnt)
{
	/* FIXME: Current design makes implementing freeing page quite challenging
	   As a hack solution it goes thorough page directory and ALL the memory mappings
	   of available mapped pages. Of course this is suboptimal.
	   As a workaround consider reusing pages in malloc/realloc/free as effectively as possible.
	 */

	if (cnt==0) cnt=1;
	unsigned int *pagemap;
	for (int i=0; i<PAGE_CNT; i++) {
		pagemap = (unsigned int*)(__page_directory[i] & ~0xF);
		if (pagemap == 0) continue;
		if ((__page_directory[i] & 0x3) != 0x3) continue;
		PageMap pm(pagemap);
		for (int j=0; j<PAGE_CNT; j++) {
			if (pm.meta.address(j)==ptr) {
				if (j+cnt>PAGE_CNT) cnt=PAGE_CNT-j;
				for (unsigned int k=0; k<cnt; k++) {
					pm.meta.setFree(j+k);
				}
				return;
			}
		}
	}
}

void PageMap::init()
{
	m.lock();

	//meta.mapPage(0, __free_page_address);
	meta.mapPage(0, __free_page_address);

	__page_directory[__paging_cnt-1] = (unsigned int)m_addr;
	__page_directory[__paging_cnt-1] |= 0x3;

	meta.initialize(__free_page_address);

	__free_page_address += PAGE_SIZE;

	m.unlock();
}

PageMap::MetaPage::MetaPage()
{
	m_addr = NULL;
}

PageMap::MetaPage::MetaPage(unsigned int *addr)
{
	m_addr = addr;
}

unsigned char *PageMap::MetaPage::map(unsigned int cnt, unsigned int *freeaddr)
{
	unsigned int f = 0;
	unsigned char *curraddr = (unsigned char*)freeaddr;

	lock();
	if (isAvailablePages(cnt)) {
		unsigned int index = count();
		for (unsigned int i=0; i<cnt; i++) {
			setUsed(index+i);
			mapPage(index+i, (unsigned int*)curraddr);
			curraddr += PAGE_SIZE;
		}
		unlock();

		return (unsigned char*)address(index);

	} else if ((f=findFreePages(cnt))>0) {
		/*
		 * FIXME TODO: this makes a hole in the memory since already mapped pages are not possibly continuous
		 * On userspace it should not matter since we can map this area as one continuous shit
		 */
		for (unsigned int i=0; i<cnt; i++) {
			setUsed(f+i);
			//mapPage(f+i, (unsigned int*)curraddr);
			//curraddr += PAGE_SIZE;
		}
		unlock();

		return (unsigned char*)address(f);
	}

	return NULL;
}

void PageMap::MetaPage::mapPage(unsigned int index, unsigned int *addr)
{
	if (m_addr==NULL) return;
	//if (index==0) return;

	m_addr[index] = (unsigned int)addr | 0x3;
}

void PageMap::MetaPage::initialize(unsigned int *addr)
{
	if (m_addr==NULL) return;

	mapPage(0, addr);
	m_index = addr;
	//m_index = (unsigned int*)m_addr[0] & ~0xF;
	m_index[0] = (unsigned int)addr;
	m_index[PAGE_MAP_CNT] = 1;
	m_index[PAGE_MAP_MUTEX] = 0;
	//m_index[3] = 0;
	m_index[3] = __paging_cnt-1;

	m = Mutex((char*)&m_index[PAGE_MAP_MUTEX]);

	m.lock();

	/* Set all pages free */
	for (unsigned int i=PAGE_MAP_INDEX; i<(PAGE_CNT+PAGE_MAP_INDEX); i++) {
		m_index[i] = 0;
	}

	setUsed(0);

	m.unlock();
}

void PageMap::MetaPage::lock()
{
	m.lock();
}

void PageMap::MetaPage::unlock()
{
	m.unlock();
}

bool PageMap::MetaPage::isAvailablePages(unsigned int cnt)
{
	if ((count()+cnt)<PAGE_CNT) return true;
	return false;
}

unsigned int PageMap::MetaPage::findFreePages(unsigned int cnt)
{
	unsigned int i=0;
	unsigned int tmp=0;
	unsigned int pos=1;

	for (i=1; i<PAGE_CNT; i++) {
		if (isFree(i)) {
			if (tmp==0) pos = i;
			tmp++;
			if (tmp>=cnt) {
				return pos;
			}
		} else {
			tmp = 0;
		}
	}

	return 0;
}

unsigned int *PageMap::MetaPage::address(unsigned int index)
{
	if (m_addr==NULL) return NULL;
#if 0
	if (index==0) {
		return m_index;
		//return (unsigned int*)m_addr[0];
	} else if (index<PAGE_CNT) {
		//return (unsigned int*)(m_addr[0]+(__paging_cnt*PAGING_SIZE));
		return (unsigned int*)(m_addr[index] & ~0xF);
	}
#else
	if (index==0) {
		return (unsigned int*)m_index[3];
	} else if (index<PAGE_CNT) {
		return (unsigned int*)((m_index[3]<<22)+index*PAGING_SIZE);
	}
#endif

	return NULL;
}

unsigned int PageMap::MetaPage::count()
{
	if (m_index==NULL) return NULL;
	return m_index[PAGE_MAP_CNT];
}

void PageMap::MetaPage::increaseCnt()
{
	if (m_addr==NULL) return;
	m_addr[PAGE_MAP_CNT]++;
}

bool PageMap::MetaPage::isFree(unsigned int index)
{
	if (m_index==NULL) return false;
	return ((unsigned char*)m_index)[PAGE_MAP_INDEX+index]==0;
}

void PageMap::MetaPage::setFree(unsigned int index)
{
	if (m_addr==NULL) return;
	if (index==0) return;
	((unsigned char*)m_index)[PAGE_MAP_INDEX+index]=0;
}

void PageMap::MetaPage::setUsed(unsigned int index)
{
	if (m_addr==NULL) return;
	((unsigned char*)m_index)[PAGE_MAP_INDEX+index]=1;
}



void paging_map_table(unsigned int *tbl,  unsigned int address)
{
	for(int i=0; i<PAGE_CNT; i++) {
		tbl[i] = address | 0x3;
		address += PAGE_SIZE;
	}
	__free_page_address = (unsigned int*)(address + PAGE_SIZE);
}

#if 0
bool paging_map_page(unsigned int *tbl,  unsigned int cnt)
{
	if (tbl==NULL) return false;
	(void)cnt;

#if 0
	/* First page is for metadata */
	unsigned int *data = tbl;

	/* Start address as first entry */
	unsigned int address = *data++;
	//data++;

	/* First free index as second value */
	unsigned int free_index = *data++;

	//data++;
	//unsigned char *mapping = (unsigned char*)data;

	if (free_index==0 || free_index>=PAGE_CNT || (free_index+cnt)>=PAGE_CNT) return false;

	for (unsigned int i=free_index; i<free_index+cnt; i++) {
		tbl[i] = address | 0x3;
		address += 4096;
	}
	data = tbl;
	//*data = address;
	data++;
	*data = (free_index+cnt+1);
#endif

	return true;
}

#endif

void *paging_alloc(unsigned int cnt)
{
	if (cnt==0) return NULL;

	/* Permanently ran out of paging tables */
	if (__paging_cnt>=PAGE_CNT) return NULL;

	unsigned int *__page_table = __page_directory + (__paging_cnt*PAGING_SIZE);

	PageMap map(__page_table);
#if 0
	if (!map.canAllocPage()) {
		__paging_cnt++;
		__page_table = __page_directory + (__paging_cnt*PAGING_SIZE);
		map = PageMap(__page_table);
		map.init();

		/* Error, can't alloc pages */
		if (!map.canAllocPage()) return NULL;
	}
#endif

	return map.map(cnt);
}

void *startmem = (void *)0x40000000;
unsigned char *mem_map = (unsigned char *)(KERNEL_VIRTUAL+0x500);
unsigned int maxmem = 0;

class Paging
{
public:
	Paging();
	void init();
	void map(void *phys, void *virt, unsigned int flags);
	
private:
	void *getPage();
};

Paging::Paging()
{
	
}

void Paging::init()
{
}

void *Paging::getPage()
{
	unsigned char t = 0;
	static unsigned int count = 0;
	unsigned char i = 0;
	while (1) {
		if ((mem_map[count] & (1<<t)) != 0) {
			mem_map[count] ^= (1<<t);
			while (t>0) {
				i++;
				t--;
			}
			return (void *)(((count*8)+i)*PAGE_SIZE);
		}

		//Last bit
		if (t==7) {
			t = 0;
			count++;
		} else {
			t++;
		}

		if (count*PAGE_SIZE >= maxmem) {
			return NULL;
		}
	}
	return NULL;
}

void Paging::map(void *phys, void *virt, unsigned int flags)
{
	unsigned int pagedir = (unsigned long)virt >> 22;
	unsigned int pagetable = (unsigned long)virt >> 12 & 0x3FF;

	unsigned long *pd = (unsigned long *) 0xFFFFF000;
        if ((pd[pagedir] & 1) != 1)
        {
                pd[pagedir] = (unsigned long) getPage() | 0x3;
        }

        unsigned long *pt = (unsigned long *) 0xFFC00000 + (0x400 * pagedir);
        if ((pt[pagetable] & 1) == 1)
        {
/*
                //printf("System Failure!\n\tPT: 0x%x\t\tnum: 0x%x", pagedir, pagetable);
                cli();
                hlt();
*/
		return;
        }

        pt[pagetable] = ((unsigned long)phys) | (flags & 0xFFF) | 0x01;
}

void paging_mmap_init(MultibootInfo *info)
{
	MemoryMap *mmap = (MemoryMap*)(info->mmap_addr + KERNEL_VIRTUAL);
	unsigned int info_end = info->mmap_addr + info->mmap_length + KERNEL_VIRTUAL;

	while ((unsigned int)(mmap) + mmap->size < info_end) {
		if ((mmap->base_addr_low + mmap->length_low) > maxmem) {
			maxmem = mmap->base_addr_low + mmap->length_low;
		}

		unsigned long addr = mmap->base_addr_low / 0x1000;
		unsigned long limit = mmap->length_low / 0x1000;

		while (addr<0x120 && limit>0) {
			addr++;
			limit--;
		}

		if (mmap->type == 1) {
			while (limit>0) {
				mem_map[addr/8] |= 1 << addr % 8;
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
		mmap = (MemoryMap *)(((unsigned int)mmap) + mmap->size + sizeof(unsigned int));
	}
}

void paging_init(MultibootInfo *info)
{
	paging_mmap_init(info);

}

#if 0
void paging_init()
{
	for(int i=0; i<PAGE_CNT; i++) {
	    //attribute: supervisor level, read/write, not present.
	    __page_directory[i] = 0 | 0x2; 
	}

	__paging_cnt = 1;
	unsigned int *__kernel_page_table = __page_directory + (__paging_cnt*PAGING_SIZE);

	paging_map_table(__kernel_page_table, 0);

	__page_directory[0] = (unsigned int)__kernel_page_table;
	__page_directory[0] |= 0x3;

	//__free_page_address = (unsigned int*)(__kernel_page_table+PAGING_SIZE+4);
	//__free_page_address = (unsigned int*)(PAGING_SIZE*(PAGE_CNT+4));
	//__free_page_address = (unsigned int*)(PAGE_SIZE*(PAGE_CNT+4));
	//PageMap kernel_map(__page_address);

	__paging_cnt++;
	unsigned int *__page_table = __page_directory + (__paging_cnt*PAGING_SIZE);
	PageMap map(__page_table);
	map.init();

	asm volatile("mov %0, %%cr3":: "b"(__page_directory));

	unsigned int cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}
#endif
