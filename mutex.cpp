#include "mutex.h"
#include "types.h"
#ifdef USE_LINUX
#include "stdio.h"
#include <pthread.h>
#endif

Mutex::Mutex()
{
	m_ptr = NULL;
}

Mutex::Mutex(ptr_val_t volatile *ptr)
{
	m_ptr = ptr;
}

Mutex::~Mutex()
{
	m_ptr = NULL;
}

int Mutex::CAS(int cmp, int set)
{
	unsigned char res;
	asm volatile (
		".1: lock; cmpxchgl %2,%1\n"
		"jnz .1\n"
		"sete %0\n"
		: "=q" (res), "=m" (*m_ptr)
		: "r" (set), "m" (*m_ptr), "a" (cmp)
		:"memory");
	return res;
}

void Mutex::lock()
{
	if (m_ptr==NULL) return;

	CAS(0, 1);
}

bool Mutex::isLocked()
{
	if (m_ptr==NULL) return false;

	if (*m_ptr==1) return true;

	return false;
}

void Mutex::unlock()
{
	if (m_ptr==NULL) return;

	// If already unlocked return
	if (*m_ptr==0) return;

	CAS(1, 0);
}

bool Mutex::wait()
{
	if (m_ptr==NULL) return false;
	//printf("Waiting\n");

	while (*m_ptr==1) {
		//Sleep
	}
	//printf("Waiting done\n");

	return true;
}

void Mutex::abort()
{
	//unlock();
}
