#ifndef MUTEX_H
#define MUTEX_H

#include "types.h"


class Mutex
{
public:
	Mutex();
	Mutex(ptr_val_t volatile *ptr);
	~Mutex();

	void lock();
	bool isLocked();
	void unlock();
	bool wait();
	void abort();

private:
	int CAS(int cmp, int set);
	ptr_val_t volatile *m_ptr;
};

#endif
