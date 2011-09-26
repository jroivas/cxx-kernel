#ifndef MM_H
#define MM_H

#include "types.h"
#include "mutex.h"


class AllocInfo;
class FreeInfo;
class MM
{
public:
	enum AllocType {
		AllocNormal = 0,
		AllocFast,
		AllocClear,
		AllocPage
	};

	MM();
	static MM *instance();
	void *alloc(size_t size, AllocType t=AllocNormal);
	bool free(void *p);

private:
	void *allocPage(size_t cnt);
	void *allocMem(size_t size, AllocType t);
	void *allocMemClear(size_t size);
	void *findAvail(size_t size);
	Mutex m;
	Mutex mf;
/*
	AllocInfo *m_root;
	FreeInfo *m_freed;
*/
};

#endif
