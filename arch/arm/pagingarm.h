#ifndef PAGING_ARM_H
#define PAGING_ARM_H

#include "types.h"
#include "paging.h"

#define PAGES_PER_TABLE 1024
#define TABLES_PER_FIRST_DIRECTORY 0x1000
#define PAGE_CNT 1024
#define PAGING_SIZE (sizeof(ptr32_t)*PAGE_CNT) // A little bit more portable
#define PAGE_SIZE PAGING_SIZE

#define PAGING_MAP_USED    (1)
#define PAGING_MAP_RW      (1<<1)
#define PAGING_MAP_USER    (1<<2)
#define PAGING_MAP_R0      (PAGING_MAP_USED|PAGING_MAP_RW)
#define PAGING_MAP_R2      (PAGING_MAP_USED|PAGING_MAP_RW|PAGING_MAP_USER)

class Page
{
public:
	enum PageType {
		KernelPage = 0,
		UserPage = 1
	};
	Page() : val(0) {}
	bool getPresent()       { return (val & 1); }
	bool getRw()            { return (val>>1) & 1; }
	bool getUserspace()     { return (val>>2) & 1; }
	bool getAccessed()      { return (val>>3) & 1; }
	bool getDirty()         { return (val>>4) & 1; }
	uint32_t getAddress()   { return (val & 0xFFFFF000); }

	void setPresent(bool is)      { if (is) val |= 1; else val &= ~1; }
	void setRw(bool is)           { if (is) val |= (1<<1); else val &= ~(1<<1); }
	void setUserspace(bool is)    { if (is) val |= (1<<2); else val &= ~(1<<2); }
	void setAccessed(bool is)     { if (is) val |= (1<<3); else val &= ~(1<<3); }
	void setDirty(bool is)        { if (is) val |= (1<<4); else val &= ~(1<<4); }
//	void setAddress(uint32_t adr) { adr>>=12; adr &= 0xFFFFF000; adr <<= 12; adr |= val&0xFFF; val = adr; }
	void setAddress(uint32_t adr) { adr &= 0xFFFFF000; adr |= val&0xFFF; val = adr; }

	bool isAvail() { return ((getAddress()==0) && (!getPresent())); }
	bool copyTo(Page *p) { if (p==nullptr) return false; p->val = val; return true; }

	void alloc(PageType type=KernelPage);
	void copyPhys(Page p);

private:
	volatile uint32_t val;
};

class PageTable
{
public:
	PageTable();
	Page *get(uint32_t i);
	bool copyTo(PageTable *table);

private:
	Page pages[PAGES_PER_TABLE];
};

class PageDir
{
public:
	enum MMUPermissions {
		MMU_NO_NO = 0,
		MMU_RO_NO,
		MMU_RO_RO,
		MMU_RW_NO,
		MMU_RW_RO,
		MMU_RW_RW
	};

	PageDir(uint32_t physLoc);
	ptr_t getPhys() { return phys; }

	//PageTable *getTable(uint32_t i);
	//Page *getPage(ptr_val_t addr, PageReserve reserve=PageDoReserve);
	//Page *getPage();
	uint32_t getPage();
	void copyTo(PageDir *dir);
	ptr32_val_t createSection(ptr32_val_t section, ptr32_val_t addr, ptr32_val_t domain, PageDir::MMUPermissions section_permissions);
	ptr32_val_t setCoarseTable(ptr32_val_t section, ptr32_val_t val);
	ptr32_val_t getCoarseTable(ptr32_val_t section);
	ptr32_val_t createCoarseTable(ptr32_val_t addr, ptr32_val_t domain, PageDir::MMUPermissions section_permissions);
	uint32_t getFreePos() { return free_pos; }
	uint32_t incFreePos() { return ++free_pos; }

private:
	uint32_t permissions(MMUPermissions permissions);
	//FirstLevelPageTable *tables[TABLES_PER_FIRST_DIRECTORY];
	//volatile uint32_t tablesPhys[TABLES_PER_DIRECTORY];
	ptr_t phys;
	uint32_t free_pos;
	uint32_t free_page_section;
	uint32_t free_page_pos;
};

void paging_mmap_init(MultibootInfo *info);

#endif
