#ifndef MM_H
#define MM_H

#include "mutex.h"
#include "types.h"


class MM
{
public:
	enum AllocType {
		AllocNormal = 0,
		AllocFast,
		AllocClear,
		AllocPage
	};
	enum AllocLock {
		AllocDoNotLock = 0,
		AllocDoLock
	};

	MM();
	static MM *instance();
	void *alloc(size_t size, AllocType t=AllocNormal);
	bool free(void *p, AllocLock l=AllocDoLock);
	void *realloc(void *ptr, size_t size);

private:
	void *allocPage(size_t cnt);
	void *allocMem(size_t size, AllocType t);
	void *allocMemClear(size_t size);
	void *findAvail(size_t size);
	Mutex m;
	void *m_lastPage;
	void *m_freeTop;
	void *m_freeMax;
};

extern "C" void *malloc(size_t size);
extern "C" void free(void *ptr);
extern "C" void *calloc(size_t cnt, size_t size);
extern "C" void *realloc(void *ptr, size_t size);


#endif
