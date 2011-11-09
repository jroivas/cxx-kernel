#ifndef PAGING_x86_H
#define PAGING_x86_H

#include "types.h"

#define PAGES_PER_TABLE 1024

class Page
{
public:
	enum PageType {
		KernelPage = 0,
		UserPage = 1
	};
	Page() : val(0) {}
	bool getPresent()       { return val & 1; }
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
	void setAddress(uint32_t adr) { adr &= 0xFFFFF000; adr |= val&0xFFF; val=adr; }

	bool isAvail() { return val==0; }
	bool copyTo(Page *p) { if (p==NULL) return false; p->val = val; return true; }

	void alloc(PageType type=KernelPage);
	void copyPhys(Page p);

private:
	uint32_t val;
};

class PageTable
{
	PageTable();
	Page *get(uint32_t i);
	bool copyTo(PageTable *table);

private:
	Page pages[PAGES_PER_TABLE];
};

#endif
