#ifndef BIOS_H
#define BIOS_H

#include "types.h"
#define MM_SLEEP (1<<0)

class BIOS
{
public:
	static BIOS *get();
	void mapMem(ptr_t addr, phys_ptr_t phys, size_t size);
	void *memMapping() { return mem_mapping; }

private:
	BIOS();
	void setupX86EMU();
	void *mem_mapping;
	void *bios_pages;
	//void *machine;
};

#endif
