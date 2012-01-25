#ifndef MUTEX_H
#define MUTEX_H

class Mutex;

#include "types.h"
#include "atomic.h"

class Mutex
{
public:
	Mutex();
	~Mutex();

	void assign(ptr_val_t volatile *ptr);
	void lock();
	void unlock();
#if 0
	inline void lock() {
		Platform_CAS(m_ptr, 0, 1);
	}
	inline void unlock() {
		Platform_CAS(m_ptr, 1, 0);
	}
#endif
	bool isLocked();
	bool wait();
	void abort();

private:
	Mutex(ptr_val_t volatile *ptr);
#if 0
	inline int CAS(int cmp, int set) {
		return Platform::CAS(m_ptr,cmp,set);
	}
#endif
	ptr_val_t volatile *m_ptr;
};

#if 1
inline void Mutex::lock() {
	if (m_ptr==NULL) return;

	//Platform_CAS(m_ptr, 0, 1);
	while (Platform_CAS(m_ptr, 0, 1)==0);
}

inline void Mutex::unlock() {
	if (m_ptr==NULL) return;

	// If already unlocked return
	if (*m_ptr==0) return;

	while (Platform_CAS(m_ptr, 1, 0)==0);
/*
	Platform_CAS(m_ptr, 1, 0);
*/
}
#endif
#endif
