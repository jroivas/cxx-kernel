#include "mm.h"

static void *current_pos = reinterpret_cast<void *>(0x02000000);

void *kmalloc(size_t size)
{
	if (size==0) return NULL;

	char *ptr = ((char*)current_pos+size);
	char *tmp = ptr;

	// This always ensures that the memory is cleared
	while (tmp<(char*)current_pos+size) {
		*tmp = 0;
		tmp++;
	}
	return (void*)ptr;
}

void kfree(void *p)
{
	(void)p;
}

void *kcalloc(unsigned int cnt, size_t size)
{
	return kmalloc(cnt*size);
}
