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
