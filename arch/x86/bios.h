#ifndef BIOS_H
#define BIOS_H

#include "types.h"
#define MM_SLEEP (1<<0)

class BIOS
{
public:
	BIOS();
	void mapMem(ptr_t addr, phys_ptr_t phys, size_t size);

private:
	void setupX86EMU();
	void *mem_mapping;
	void *bios_pages;
};

#endif
