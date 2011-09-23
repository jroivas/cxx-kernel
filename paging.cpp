#include "paging.h"
#include "types.h"
#include "mutex.h"

#define PAGE_CNT 1024
//#define VIRT_TO_PHYS 0x40000000
#define LPOS 0x100000
//#define PAGING_POS 0x10000
#define PAGING_START_POS 0x10000
#define PAGE_SIZE 4096

#define PAGE_MAP_CNT 1
#define PAGE_MAP_MUTEX 2
#define PAGE_MAP_INDEX 4

//#define PAGING_SIZE 0x1000
#define PAGING_SIZE (sizeof(unsigned int*)*PAGE_CNT) // A little bit more portable

unsigned int *__page_directory = (unsigned int*) 0x10000; //TODO FIXME
int __paging_cnt = 0;
unsigned int *__free_page_address = (unsigned int*)PAGING_START_POS;

class PageMap
{
public:
	PageMap(unsigned int *addr);
	void init();
	//void initialize(unsigned int *addr);
	bool canAllocPage();
	unsigned char *map(unsigned int cnt);
	void forceMap(unsigned int i);

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
};

PageMap::PageMap(unsigned int *addr)
{
	m_addr = addr;
	meta = MetaPage(m_addr);
}

void PageMap::forceMap(unsigned int i)
{
	meta.lock();
	meta.setUsed(i);
	meta.unlock();
}

bool PageMap::canAllocPage()
{
	if (meta.isAvailablePages(1)) {
		return true;
	}
	if (meta.findFreePages(1)>0) {
		return true;
	}

	return false;
}

unsigned char *PageMap::map(unsigned int cnt)
{
	unsigned char *tmp = meta.map(cnt, __free_page_address);
	if (tmp!=NULL) {
		__free_page_address += (PAGE_SIZE*cnt);
	}
	return tmp;
}

void PageMap::init()
{
	meta.mapPage(0, __free_page_address);
	meta.initialize(__free_page_address);

	__free_page_address += PAGE_SIZE;
}

PageMap::MetaPage::MetaPage()
{
	m_addr = NULL;
}

PageMap::MetaPage::MetaPage(unsigned int *addr)
{
	m_addr = addr;
#if 0
	m_addr_page = addr;
	m_addr = NULL;
#endif
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
		for (unsigned int i=0; i<cnt; i++) {
			setUsed(f+i);
			mapPage(f+i, (unsigned int*)curraddr);
		}
		unlock();

		return (unsigned char*)address(f);
	}

	return NULL;
}

void PageMap::MetaPage::mapPage(unsigned int index, unsigned int *addr)
{
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
	m_index[3] = 0;

	for (unsigned int i=PAGE_MAP_INDEX; i<PAGE_CNT; i++) {
		m_index[i] = 0;
	}

	m = Mutex((char*)&m_index[PAGE_MAP_MUTEX]);
	setUsed(0);

#if 0
	if (m_addr_page==NULL) return;

	m_addr = addr;
	m_addr[0] = (unsigned int)addr;
	m_addr[1] = 1;
	m_addr[2] = 0;
	m = Mutex((char*)&m_addr[2]);
	setUsed(0);
#endif
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
	if (index==0) {
		return m_index;
		//return (unsigned int*)m_addr[0];
	} else if (index<PAGE_CNT) {
		//return (unsigned int*)(m_addr[0]+(__paging_cnt*PAGING_SIZE));
		return (unsigned int*)(m_addr[index] & ~0xF);
	}

	return NULL;
}

unsigned int PageMap::MetaPage::count()
{
	if (m_index==NULL) return NULL;
	return m_index[PAGE_MAP_CNT];
#if 0
	if (m_addr==NULL) return NULL;
	return m_addr[1];
#endif
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
}

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


void *paging_alloc(unsigned int cnt/*, unsigned int *address*/)
{
	if (cnt==0) return NULL;

	if (__paging_cnt>=PAGE_CNT) return NULL;

	unsigned int *__page_table = __page_directory + (__paging_cnt*PAGING_SIZE);

	//__free_page_address = (unsigned int*)(4096*(PAGE_CNT+__paging_cnt));
	//__free_page_address = (unsigned int*)((unsigned int)(__free_page_address)+(PAGE_SIZE*cnt));

	PageMap map(__page_table);
	if (!map.canAllocPage()) {
		__paging_cnt++;
		__page_table = __page_directory + (__paging_cnt*PAGING_SIZE);
		map = PageMap(__page_table);
		map.init();
	}
	//map.initialize(address);
	return map.map(cnt);

	//paging_map_table(__page_table, address);
}

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

	__paging_cnt++;
	__free_page_address = (unsigned int*)(PAGE_SIZE*(PAGE_CNT+4));
	//PageMap kernel_map(__page_address);

	asm volatile("mov %0, %%cr3":: "b"(__page_directory));

	unsigned int cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}
