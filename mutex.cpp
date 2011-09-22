#include "mutex.h"
#include "types.h"

Mutex::Mutex(void *ptr)
{
	m_ptr = (char*)ptr;
}

Mutex::~Mutex()
{
	m_ptr = NULL;
}

void Mutex::lock()
{
	if (m_ptr==NULL) return;

	if (isLocked()) wait();

	*m_ptr = 1;
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

	*m_ptr = 0;
}

bool Mutex::wait()
{
	if (m_ptr==NULL) return false;

	while (*m_ptr==1) {
		//Sleep
	}
	return true;
}

void Mutex::abort()
{
	unlock();
}
