#include "pagingx86.h"
#include "paging.h"

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
	for (int i=0; i<PAGES_PER_TABLE; i++) {
		pages[i] = Page();
	}
}

Page *PageTable::get(uint32_t i)
{
	if (i<PAGES_PER_TABLE) return &pages[i];
	return NULL;
}

bool PageTable::copyTo(PageTable *table)
{
	if (table==NULL) return false;

	Paging p;
	for (int i=0; i<PAGES_PER_TABLE; i++) {
		if (pages[i].isAvail()) {
			table->pages[i].alloc();
			pages[i].copyTo(&table->pages[i]);
			pages[i].copyPhys(table->pages[i]);
		}
	}

	return true;
}
