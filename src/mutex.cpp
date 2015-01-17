#include "mutex.h"
#include "types.h"
#include "arch/platform.h"

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

void Mutex::assign(ptr_val_t volatile *ptr)
{
    m_ptr = ptr;
}

#if 0
int Mutex::CAS(int cmp, int set)
{
    return Platform::CAS(m_ptr,cmp,set);
}

void Mutex::lock()
{
    if (m_ptr==NULL) return;

    while (CAS(0, 1)==0);
}

void Mutex::unlock()
{
    if (m_ptr==NULL) return;

    // If already unlocked return
    if (*m_ptr==0) return;

    while (CAS(1, 0)==0);
}
#endif

bool Mutex::isLocked()
{
    if (m_ptr==NULL) return false;

    if (*m_ptr==1) return true;

    return false;
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
    //unlock();
}
