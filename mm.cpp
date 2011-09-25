#include "mm.h"
#include "paging.h"

//static void *current_pos = reinterpret_cast<void *>(0x02000000);

void *kmalloc(size_t size)
{
	if (size==0) return NULL;
	int cnt = size/4096+1; //FIXME
	//int cnt = size/4096; //FIXME

	char *pos = (char*)paging_alloc(cnt);
	if (pos==NULL) return NULL;

	//char *ptr = ((char*)current_pos+size);
	char *ptr = ((char*)pos);
	char *tmp = ptr;

	// This always ensures that the memory is cleared
	while (tmp<(char*)pos+size) {
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
